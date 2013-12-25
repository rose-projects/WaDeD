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
 * @file  tree.h
 * @brief Handle the Merkel trees.
 */

#ifndef __TREE_H__
#define __TREE_H__

#define HAS 1
#define HAS_NOT 0
#define END_REACHED 0xFFFF

#include "memory.h"
#include "hash.h"

/**
 * @brief Construct the tree from the state of the FRAM.
 */
void build_tree(void);

/**
 * @brief Insert a bucket in memory and update the tree accordingly.
 *
 * @param b A pointer to the bucket to be inserted.
 *
 * @return The address of the newly inserted bucket in FRAM.
 */
uint16_t tree_insert(struct Bucket *b);

#ifdef __MEMTESTS__
/**
 * @brief Return the address in RAM of the right tree.
 *
 * @return A pointer to the root of the right tree.
 */
uint64_t* get_r(void);

/**
 * @brief Return the address in RAM of the lest tree.
 *
 * @return A pointer to the root of the left tree.
 */
uint64_t* get_l(void);

#endif // __MEMTESTS__

/**
 * @brief Reset the leaves of the tree at 0, and the nodes accordingly.
 */
void tree_clean(void);

/**
 * @brief Put the hashes of the two roots in the given buffer.
 *
 * @param buf A pointer to a 16 octets buffer.
 */
void tree_get_roots(void *buf);

/**
 * @brief Put the hashes of the node and the node sons in the given buffers.
 *
 * @param n the given node.
 * @param f A pointer to the node hash buffer.
 * @param s A pointer to the sons hashes buffer.
 */
void get_hash_and_sons(uint8_t n, uint64_t **f, uint64_t **s);

/**
 * @brief Return the hash of a given leaf of the tree.
 *
 * @param leaf The leaf we want to have a hash of.
 *
 * @return The hash of the leaf.
 */
uint64_t get_leaf_hash(uint16_t leaf);

/**
 * @brief Put the hashes of a given list in a buffer.
 *
 * @param leaf The leaf pointing to the list.
 * @param buffer The buffer in which to store the hashes.
 *
 * @return The number of 8-bytes hashes put in the buffer.
 */
int get_list(uint16_t leaf, uint64_t *buffer);

/**
 * @brief Compare the hashes of a received list and the internal list.
 *
 * @param leaf The leaf pointing to the list.
 * @param buf A buffer containing the hashes of the distant list.
 * @param list_size The size of the distant list.
 *
 * @return The number of messages to be sent.
 *
 * The first hash of the distant list is located at buf + 8. The function
 * places the addresses of the buckets to be sent in the buffer on 16-bits
 * integers, starting at buf.
 */
uint8_t cmp_lists(uint16_t leaf, void *buf, uint8_t list_size);

int tree_has_message(uint64_t id);

void tree_reset(void);

/**
 * @brief Gets the next bucket meant for a given id.
 *
 * @param id The destination address.
 * @param start The place in memory we are looking at.
 * @param buf The buffer to be filled.
 *
 * This looks in memory and fills the buffer with a bucket whose message
 * has id as its destination address. If (buf->state & 1) is unset, it
 * means no message has been found.
 */
uint16_t next_message(uint16_t id, uint16_t start, struct Bucket *buf);

#endif // __TREE_H__
