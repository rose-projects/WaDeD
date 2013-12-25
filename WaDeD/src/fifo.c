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
 * @file  fifo.c
 * @brief Handle a fifo to determine to future messages to send.
 */

#include <string.h>
#include "fifo.h"
#include "tree.h"
#include "jungle.h"

static const int PROTOCOL_VERSION = 0;
extern int DEVICE_ID;
#ifndef __TAG_MODE__
#define FIRST_BYTE PROTOCOL_VERSION
#else
// In this mode, FIRST_BYTE serves to identify the device.
#define FIRST_BYTE DEVICE_ID
#endif

#ifndef __SIMU__

/**
 * @brief The fifo (bits 15-12: transaction type, 11-0: argument).
 */
static uint16_t  fifo [FIFO_MAXSIZE];

uint8_t  tx_buffer[164];
static int       fifo_head     = 0; /**< Fifo head. */
static int       fifo_tail     = 0; /**< Fifo tail. */
static int       fifo_size     = 0; /**< Fifo size. */
static systime_t fifo_deadline = 0; /**< Deadline to a new ROOT message. */

#else
extern uint8_t  *tx_buffer;
extern uint16_t *fifo;
extern int       fifo_head;
extern int       fifo_tail;
extern int       fifo_size;
#endif // __SIMU__

/**
 * @brief Prepare a packet, ready to be sent, containing a node and its sons.
 *
 * @param n node indicator.
 *
 * The 7 weakest bits of n indicate the place of the node in the tree. The
 * strongest bit indicates the tree (0 for left, 1 for right).
 *
 * Length should be fixed at 73 bytes:
 * byte 0: n
 * bytes 1-8: node hash
 * bytes 9-72: sons hashes, in order from left to right
 *
 * The packet header should be 00000000, followed by an octet indicating the
 * size of the packet, which is 73.
 */
static void prepare_node(uint8_t n)
{
    tx_buffer[0] = (FIRST_BYTE << 4) | NODE;
    tx_buffer[1] = 73;
    tx_buffer[2] = n;

    uint64_t *hash, *son_hashes;
    get_hash_and_sons(n, &hash, &son_hashes);
    memcpy(tx_buffer + 3, hash, 8);
    for (int i = 0; i < 8; i ++)
        memcpy(tx_buffer + 11 + 8*i, son_hashes + i, 8);
}

/**
 * @brief Prepare a packet, ready to be sent, containing a list.
 *
 * @param l identifier of the leaf to be sent.
 *
 * Length varies:
 * bytes 0-1: leaf identifier (10 bits), number of elements in the list (6)
 * bytes 2-9: list hash
 * bytes 10+: hashes of each element in the list
 *
 * The packet header should be 00000001, followed by an octet indicating the
 * packet size, which is 10 + 8 * (number of elements in the list).
 */
static void prepare_leaf(uint16_t l)
{
    tx_buffer[0] = (FIRST_BYTE << 4) | LEAF;

    uint64_t hash = get_leaf_hash(l);
    memcpy(tx_buffer + 4, &hash, 8);
    int length = get_list(l, (uint64_t *) (tx_buffer + 12));
    *((uint16_t *) (tx_buffer + 2)) = (l << 6) | length;
    tx_buffer[1] = 10 + 8 * length;

    // TODO maybe do something in the rare case of a long list (> 18 elements)
}

/**
 * @brief Prepare a packet containing the roots of the tree, ready to be sent.
 *
 * Length should be fixed at 16 bytes:
 * bytes 0-7: top hash of the left tree.
 * bytes 8-15: top hash of the right tree.
 *
 * The packet header should be 00000002, followed by an octet indicating the
 * packet size, which is always 16.
 */
static void prepare_roots(void)
{
    tx_buffer[0] = (FIRST_BYTE << 4) | ROOT;
    tx_buffer[1] = 16;
    tree_get_roots(&tx_buffer[2]);
}

/**
 * @brief Prepare a packet containing a message, ready to be sent.
 *
 * @param address the address of the message in memory.
 *
 * msg should be built the following way:
 * bytes 0-7: hash
 * bytes 8-11: emission date
 * bytes 12-15: expiration date
 * bytes 16-17: source address
 * bytes 18-19: destination address
 * bytes 20+: content of the message, up to 140 characters
 *
 * The packet header should be 00000003 (protocol version, packet identifier),
 * followed by an octet indicating the length of the packet, stripped of the
 * header(for an empty message, that would be 20).
 */
static void prepare_message(uint16_t address)
{
    tx_buffer[0] = (FIRST_BYTE << 4) | MESSAGE;

    struct Bucket bucket;
    read_bucket(address, &bucket);
    memcpy(tx_buffer +  2, &bucket.type.id, 8);
    *((uint32_t *) (tx_buffer + 10)) = bucket.emission_date;
    *((uint32_t *) (tx_buffer + 14)) = bucket.expiration_date;
    *((uint16_t *) (tx_buffer + 18)) = bucket.source_address;
    *((uint16_t *) (tx_buffer + 20)) = bucket.destination_address;
    int i = 0;
    while (bucket.message[i]) {
        tx_buffer[22 + i] = bucket.message[i];
        i++;
    }
    tx_buffer[1] = 20 + i;
}

/**
 * @brief Remove the first element of the fifo. Do not use if fifo is empty.
 */
static inline void pop(void)
{
#ifndef __LIFO__
    fifo_head = (fifo_head + 1) % FIFO_MAXSIZE;
#else
    fifo_tail = (fifo_tail ? (fifo_tail - 1) : (FIFO_MAXSIZE - 1));
#endif // __LIFO__

    fifo_size--;
}

/**
 * @brief Put an element at the end of the fifo. Do not use if fifo is full.
 *
 * @param i The element to insert.
 */
static inline void push(uint16_t i)
{
    fifo[fifo_tail] = i;
    fifo_tail = (fifo_tail + 1) % FIFO_MAXSIZE;
    fifo_size++;
}

/**
 * @brief Determine if an element is in the fifo.
 *
 * @param i The element to find.
 *
 * @return 1 if found, 0 if not.
 */
static int fifo_has(uint16_t i)
{
    for (int j = 0; j < fifo_size; j++)
        if (fifo[(fifo_head + j) % FIFO_MAXSIZE] == i)
            return 1;
    return 0;
}

/**
 * @brief Prepare the next message to be sent.
 */
int fifo_pop(void)
{
    unless(fifo_size) {
#ifndef __SIMU__
        if ((int32_t) (fifo_deadline + S2ST(2) - chTimeNow()) < 0) {
            fifo_deadline = chTimeNow();
#endif // __SIMU__
            prepare_roots();
            return 1;
#ifndef __SIMU__
        } else
            return 0;
#endif // __SIMU__
    }

#ifndef __LIFO__
    uint8_t type = (uint8_t) (fifo[fifo_head] >> 12);
    uint16_t arg = fifo[fifo_head] & 0x0FFF;
    pop();
#else
    uint8_t type = (uint8_t) (fifo[fifo_tail] >> 12);
    uint16_t arg = fifo[fifo_tail] & 0x0FFF;
    pop();
#endif // __LIFO__

    switch (type) {
        case NODE:
            prepare_node(arg);
            break;
        case ROOT:
            prepare_roots();
            break;
        case LEAF:
            prepare_leaf(arg);
            break;
        case MESSAGE:
            prepare_message(arg);
            break;
    }
    return 1;
}

/**
 * @brief Push the command for a message to send in the FIFO.
 *
 * @param arg    Arguments related to tthe type of the message.
 * @param type   Type of the messsage.
 */
void fifo_push(uint16_t arg, uint8_t type)
{
#ifndef __LIFO__
    if(fifo_size == FIFO_MAXSIZE)
        pop();
#else
    if(fifo_size == FIFO_MAXSIZE) {
        fifo_head = (fifo_head + 1) % FIFO_MAXSIZE;
        fifo_size--;
    }
#endif // __LIFO__

    uint16_t i = (arg & 0x0FFF) + (type << 12);
    unless(fifo_has(i))
        push(i);
}
