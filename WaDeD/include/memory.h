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
 * @file  memory.h
 * @brief Handle the memory in FRAM implementing a memory pool.
 */

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include <stddef.h>

#include "fram.h"
#include "hash.h"
#include "bucket.h"

#define unless(x) if(!(x))

#define MEM_FULL 0xFFFF

#ifdef LAB_BOARD
#define MEM_SIZE 256
#else
#define MEM_SIZE 1024
#endif // LAB_BOARD

#ifndef __SMALL_TREE__
#define TREE_SIZE 1024
#else
#define TREE_SIZE 512
#endif

#define BUCKET_SIZE sizeof(struct Bucket)
#define HANDLER_SIZE sizeof(uint16_t)

#define HANDLERS_SECTION_SIZE TREE_SIZE
#define HANDLERS_START 0
#define HANDLERS_END (HANDLERS_START \
        + HANDLERS_SECTION_SIZE * HANDLER_SIZE)

#define BUCKETS_SECTION_SIZE MEM_SIZE
#define BUCKETS_START HANDLERS_END
#define BUCKETS_END (BUCKETS_START + BUCKETS_SECTION_SIZE * BUCKET_SIZE)

/**
 * @brief Reads a bucket from memory.
 *
 * @param address The address to read from.
 * @param bucket A pointer to the bucket to store read data.
 */
static inline void read_bucket(uint16_t address, struct Bucket *bucket)
{
    fram_read(BUCKETS_START + BUCKET_SIZE * address, \
            (uint8_t *) bucket, \
            BUCKET_SIZE);
}

/**
 * @brief Find the head of the list that a handler is pointing to.
 *
 * @param leaf_id Handler ID.
 *
 * @return Bucket address.
 */
static inline uint16_t read_list_address(uint16_t leaf_id)
{
    return fram_read16(HANDLERS_START + HANDLER_SIZE * leaf_id);
}

/**
 * @brief Find the list in which a message should belong.
 *
 * @param id Hash of the message.
 *
 * @return Handler ID.
 */
static inline uint16_t small_id(uint64_t id)
{
#ifndef __SMALL_TREE__
    return (uint16_t) (id >> 54);
#else
    return (uint16_t) (id >> 55);
#endif // __SMALL_TREE__
}

#define BUCKET_READ_FIELD(address, field, bits) \
    fram_read ## bits (BUCKETS_START \
            + BUCKET_SIZE * (address) \
            + offsetof(struct Bucket, field))

#define BUCKET_WRITE_FIELD(address, field, bits, data) \
    fram_write ## bits (BUCKETS_START \
            + BUCKET_SIZE * (address) \
            + offsetof(struct Bucket, field), \
            data)

/**
 * @brief Given leaf, returns the address of the first packet of the list.
 *
 * @param id Given leaf.
 *
 * @return First packet of the list.
 */
uint16_t memory_get_ids_head(uint16_t id);

/**
 * @brief Returns the head of the timestamps list.
 *
 * @return Head of the timestamps list.
 */
uint16_t memory_get_timestamps_head(void);

/**
 * @brief Insert new_packet into the memory.
 *
 * @param new_packet The packet to insert.
 *
 * @return address of the packet in the table.
 */
uint16_t memory_insert_bucket(struct Bucket *new_bucket);

/**
 * @brief Marks the given address as free.
 *
 * @param address The address to mark as free.
 */
void memory_erase_bucket(uint16_t address);

/**
 * @brief Clean FRAM, leaving it ready for use.
 */
void memory_clean(void);

/**
 * @brief Computes the hash of a leaf, depending on the associated list.
 *
 * @param id Handler identifier.
 *
 * @return The hash of the list. 0 if there is no element in the list.
 */
uint64_t memory_list_hash(uint16_t id);

uint64_t memory_hash_bucket(struct Bucket *buf);

#endif // __MEMORY_H__
