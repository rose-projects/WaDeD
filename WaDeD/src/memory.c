/*
 *  WaDeD - Short messages mesh network
 *
 *  Copyright (C) 2013 WaDeD-ROSE <waded-rose@googlegroups.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file  memory.c
 * @brief Gestion of the memory.
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "memory.h"

#define NO_NEXT 0xFFFF
#define NO_BUCKET 0xFFFF

#define HAS 1
#define HAS_NOT 0

#define UP 1
#define DOWN 0

#define LIST_HAS_CHANGED 1
#define LIST_HAS_NOT_CHANGED 0

#ifndef __SIMU__
/**
 * @brief Head of the freelist.
 *
 * Initialized at fram_init or memory_clean.
 */
static uint16_t freelist_head;

/**
 * @brief Head of the timestamps list.
 *
 * Point to the packet that should be suppressed first.
 */
static uint16_t timestamps_head;

/**
 * @brief Keeps track of the number of stored packets.
 */
uint16_t memory_counter;
#else // __SIMU__
extern uint16_t freelist_head;
extern uint16_t timestamps_head;
extern uint16_t memory_counter;
#endif // __SIMU__

/**
 * @brief Insert a bucket at a given address.
 *
 * @param address The address at which to insert the bucket.
 * @param bucket A pointer to the bucket to be inserted.
 */
static inline void put_bucket(uint16_t address, struct Bucket *bucket)
{
    fram_write(BUCKETS_START + BUCKET_SIZE * address, \
            (uint8_t *) bucket, \
            BUCKET_SIZE);
}

static inline void write_list_address(uint16_t id, uint16_t address)
{
    fram_write16(HANDLERS_START + HANDLER_SIZE * id, address);
}

static inline uint16_t read_next_id(uint16_t position)
{
    return BUCKET_READ_FIELD(position, next_id, 16);
}

static inline void write_next_id(uint16_t position, uint16_t address)
{
    BUCKET_WRITE_FIELD(position, next_id, 16, address);
}

static inline uint16_t get_next_timestamp(uint16_t position)
{
    return BUCKET_READ_FIELD(position, next_timestamp, 16);
}

static inline void set_next_timestamp(uint16_t position, uint16_t address)
{
    BUCKET_WRITE_FIELD(position, next_timestamp, 16, address);
}

/**
 * @brief Changes a bit of the state byte of a given bucket in fram.
 *
 * @param address The bucket to modify.
 * @param bit The bit to be changed (0 = weakest, 7 = strongest).
 * @param up Value for the bit to be set at.
 */
static void set_state(uint16_t address, uint8_t bit, uint8_t up)
{
    uint8_t mask = (1 << bit);
    uint8_t new_state = BUCKET_READ_FIELD(address, state, 8);
    up ? (new_state |= mask) : (new_state &= ~mask);
    BUCKET_WRITE_FIELD(address, state, 8, new_state);
}

uint16_t memory_get_ids_head(uint16_t id)
{
    return read_list_address(id);
}

uint16_t memory_get_timestamps_head(void)
{
    return timestamps_head;
}

/**
 * @brief Reserve a space in memory to be allocated.
 *
 * @return The address of the allocated bucket. MEM_FULL if it failed.
 */
static uint16_t allocate_bucket(void)
{
    if(memory_counter >= BUCKETS_SECTION_SIZE)
        return MEM_FULL;

    uint16_t new_bucket_address = freelist_head;

    // Update the free list.
    freelist_head = BUCKET_READ_FIELD(freelist_head, type.empty.next,
            16);
    BUCKET_WRITE_FIELD(freelist_head, type.empty.first, 8, 1);

    memory_counter++;

    return new_bucket_address;
}

/**
 * @brief Adds a bucket to the free list.
 *
 * @param address The bucket to be given back.
 */
static void add_to_freelist(uint16_t address)
{
    // Unmark the previous head of the free list as first.
    BUCKET_WRITE_FIELD(freelist_head, type.empty.first, 8, 0);

    // Write the new packet head.
    BUCKET_WRITE_FIELD(address, type.empty.next, 16, freelist_head);
    BUCKET_WRITE_FIELD(address, type.empty.first, 8, 1);

    // The erased packet becomes the new head.
    freelist_head = address;
}

/**
 * @brief Insert a bucket in the list of hashes.
 *
 * @param position Previous packet in the list.
 * @param bucket_address Address where the bucket will be inserted.
 * @param b Bucket to be modified.
 */
static void rec_insert_in_ids(uint16_t position,
        uint16_t bucket_address, struct Bucket *b)
{
    // Fetch next bucket in the list.
    uint16_t next = read_next_id(position);

    // If we have reached the end of the list, insert it here.
    if(next == NO_NEXT) {
        b->next_id = NO_NEXT;
        write_next_id(position, bucket_address);
        return;
    }

    // Otherwise, compare and call the function recursively if needed.
    uint64_t id = BUCKET_READ_FIELD(next, type.id, 64);
    if(id >= b->type.id) {
        b->next_id = next;
        write_next_id(position, bucket_address);
    } else
        rec_insert_in_ids(next, bucket_address, b);
}

/**
 * @brief Arrange values in memory so that a bucket is correcly inserted.
 *
 * @param bucket_address The address where the bucket will be inserted.
 * @param b The bucket to be inserted.
 */
static void insert_in_ids(uint16_t bucket_address, struct Bucket *b)
{
    uint16_t leaf = small_id(b->type.id);
    uint16_t position = read_list_address(leaf);

    // If the list is empty, insert it.
    if(position == NO_BUCKET) {
        b->next_id = NO_NEXT;
        b->state |= 0x04;
        write_list_address(leaf, bucket_address);
    } else if(BUCKET_READ_FIELD(position, type.id, 64)
            >= b->type.id) {
        // If it is not empty, but the position should be first.
        set_state(position, 2, DOWN);

        b->next_id = position;
        b->state |= 0x04;
        write_list_address(leaf, bucket_address);
    } else {
        b->state &= ~0x04;
        rec_insert_in_ids(position, bucket_address, b);
    }
}

/**
 * @brief Correctly insert a message in the timestamps list.
 *
 * @param position The previous element in the list.
 * @param address The address where the bucket will be inserted.
 * @param bucket The bucket to be inserted.
 */
static void rec_insert_in_timestamps(uint16_t position, uint16_t address,
        struct Bucket *bucket)
{
    uint16_t next = get_next_timestamp(position);

    // If we have reached the end of the list, insert it there.
    if(next == NO_NEXT) {
        bucket->next_timestamp = NO_NEXT;
        BUCKET_WRITE_FIELD(position, next_timestamp, 16, address);
        return;
    }

    // Checks whether it is the right position and act accordingly.
    uint32_t ts = BUCKET_READ_FIELD(next, expiration_date, 32);
    if(ts >= bucket->expiration_date) {
        bucket->next_timestamp = next;
        set_next_timestamp(position, address);
    } else
        rec_insert_in_timestamps(next, address, bucket);
}

static void insert_in_timestamps(uint16_t address, struct Bucket *bucket)
{
    if(timestamps_head == NO_NEXT) { // empty list
        bucket->next_timestamp = NO_NEXT;
        bucket->state |= 0x02;
        timestamps_head = address;
    } else if(BUCKET_READ_FIELD(timestamps_head, expiration_date, 32)
            >= bucket->expiration_date) {

        // Update the status of the previous head
        set_state(timestamps_head, 1, DOWN);

        bucket->next_timestamp = timestamps_head;
        bucket->state |= 0x02;
        timestamps_head = address;
    } else {
        bucket->state &= ~0x02;
        rec_insert_in_timestamps(timestamps_head, address, bucket);
    }
}

/**
 * @brief Insert a new bucket in memory and updates memory state accordingly.
 * The bucket should not be here already.
 *
 * @param new_bucket The bucket to be inserted.
 *
 * @return The address of the new bucket, or MEM_FULL if insertion failed.
 */
uint16_t memory_insert_bucket(struct Bucket * new_bucket)
{
    // Fetch the address that will be given to the bucket.
    uint16_t new_bucket_address = allocate_bucket();
    if(new_bucket_address == MEM_FULL)
        return MEM_FULL;

    // Mark the bucket as used.
    new_bucket->state |= 0x01;

    // Update lists.
    insert_in_timestamps(new_bucket_address, new_bucket);
    insert_in_ids(new_bucket_address, new_bucket);

    // Write the bucket in memory.
    put_bucket(new_bucket_address, new_bucket);

    return new_bucket_address;
}

/**
 * @brief Recursively suppresses a message from its id list.
 *
 * @param address The bucket to be erased.
 * @param position The previous bucket in the list.
 */
static void rec_erase_from_ids(uint16_t address, uint16_t position)
{
    uint16_t next = read_next_id(position);

    if(next == NO_NEXT)
        return;

    if(next == address) {
        uint16_t nnext = read_next_id(next);
        BUCKET_WRITE_FIELD(position, next_id, 16, nnext);
    } else
        rec_erase_from_ids(address, next);
}

/**
 * @brief Suppresses a message from its id list.
 *
 * @param address The bucket to be erased.
 */
static void erase_from_ids(uint16_t address)
{
    uint16_t leaf = small_id(BUCKET_READ_FIELD(address, type.id, 64));
    uint16_t head = read_list_address(leaf);

    if(head == address) {
        uint16_t new_head = read_next_id(head);
        write_list_address(leaf, new_head);
        if(new_head != NO_NEXT)
            set_state(new_head, 2, UP);
    } else
        rec_erase_from_ids(address, head);
}

/**
 * @brief Recursively suppresses a message from its timestamps list.
 *
 * @param address The bucket to be erased.
 * @param position The previous bucket in the list.
 */
static void rec_erase_from_timestamps(uint16_t address, uint16_t position)
{
    uint16_t next = get_next_timestamp(position);

    if(next == NO_NEXT)
        return;

    if(next == address) {
        uint16_t nnext = get_next_timestamp(next);
        set_next_timestamp(position, nnext);
    } else
        rec_erase_from_timestamps(address, next);
}

/**
 * @brief Suppresses a message from the timestamps list.
 *
 * @param address The previous bucket in the list.
 */
static void erase_from_timestamps(uint16_t address)
{
    if(timestamps_head == address) {
        timestamps_head = BUCKET_READ_FIELD(address,
                next_timestamp, 16);
        if(timestamps_head != NO_BUCKET)
            set_state(timestamps_head, 1, UP);
    } else
        rec_erase_from_timestamps(address, timestamps_head);
}

/**
 * @brief Free a bucket from memory.
 *
 * @param address The bucket to be freed.
 */
void memory_erase_bucket(uint16_t address)
{
    if(!memory_counter)
        return;

    // Mark the packet as empty.
    BUCKET_WRITE_FIELD(address, state, 8, 0);

    // Update memory state.
    erase_from_timestamps(address);
    erase_from_ids(address);
    add_to_freelist(address);

    memory_counter--;

    if(!memory_counter)
        timestamps_head = NO_BUCKET;
}

void memory_clean(void)
{
    //USE_MEMORY
    struct Bucket b;

    memset(&b, 0, sizeof b);

    b.type.empty.first = 1;
    b.type.empty.next  = 1;
    put_bucket(0, &b);

    for(int i = 1 ; i < BUCKETS_SECTION_SIZE - 1 ; i++) {
        b.type.empty.first = 0;
        b.type.empty.next = i + 1;
        put_bucket(i, &b);
    }

    b.type.empty.first = 0;
    b.type.empty.next = NO_NEXT;
    put_bucket(BUCKETS_SECTION_SIZE - 1, &b);

    for(int i = 0 ; i < HANDLERS_SECTION_SIZE ; i++)
        fram_write16(HANDLERS_START + HANDLER_SIZE * i,
                NO_BUCKET);

    memory_counter = 0;
    freelist_head = 0;
    timestamps_head = NO_BUCKET;
}

uint64_t memory_list_hash(uint16_t id)
{
    uint16_t address = read_list_address(id);

    //USE_MEMORY
    uint64_t buf [10];
    buf[0] = 0;

    while(address != NO_NEXT) {
        for(int i = 1 ; i < 10 ; i++) {
            buf[i] = BUCKET_READ_FIELD(address, type.id, 64);
            address = BUCKET_READ_FIELD(address, next_id, 16);
            if(address == NO_NEXT) {
                buf[0] = hash(buf, 8*(i+1));
                return buf[0];
            }
        }
        buf[0] = hash(buf, 80);
    }

    return buf[0];
}

/**
 * @brief Initializes the RAM according to the data stored in FRAM.
 */
void memory_init(void)
{
    memory_counter = 0;
    timestamps_head = NO_BUCKET;
    freelist_head = NO_BUCKET;

    for(int i = 0 ; i < MEM_SIZE ; i++) {
        uint8_t state = BUCKET_READ_FIELD(i, state, 8);
        if(state & 0x01) {
            memory_counter++;
            if(state & 0x02)
                timestamps_head = i;
            continue;
        }
        if(BUCKET_READ_FIELD(i, type.empty.first, 8)) {
            freelist_head = i;
            continue;
        }
    }
}

uint64_t memory_hash_bucket(struct Bucket *buf)
{
    //USE_MEMORY
    uint8_t tmp[152];
    memcpy(tmp, &(buf->emission_date), 12);
    strncpy((char *) (tmp + 12), (char *) &(buf->message), 140);
    return hash(tmp, 12 + strlen((char *) buf->message));
}
