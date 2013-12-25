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

#include "tree.h"
#include <string.h>

#include "assert.h"
#include "ch.h"

#define NO_BUCKET  0xFFFF
#define NO_NEXT    0xFFFF
#define HAS_BUCKET 0xFFFE

#define NNODES  73
#define NLEAVES 512

MUTEX_DECL(tree_mtx);

#ifndef __SIMU__
//USE_MEMORY
static uint64_t ltree [NNODES + NLEAVES];
#ifdef __SMALL_TREE__
static uint64_t rtree [1];
#else
static uint64_t rtree [NNODES + NLEAVES];
#endif // __SMALL_TREE__

#else
extern uint64_t *ltree;
extern uint64_t *rtree;
#endif // __SIMU__

static inline uint64_t* get_tree(uint16_t leaf)
{
#ifndef __SMALL_TREE__
    return (leaf < NLEAVES) ? ltree : rtree;
#endif
    (void) leaf;
    return ltree;
}

/**
 * @brief Compute the hash of a node.
 *
 * @param node The node of which we want to have the hash.
 * @param subtree The tree containing the node.
 */
static void make_node(uint16_t node, uint64_t *subtree)
{
    //USE_MEMORY
    uint64_t buf [8];

    for(int i = 0 ; i < 8 ; i++)
        buf[i] = subtree[8 * node + i + 1];

    subtree[node] = hash(buf, 64);
}

/**
 * @brief Compute the internal hashes of a tree.
 *
 * @param subtree The tree to compute.
 */
static void make_subtree(uint64_t *subtree)
{
    for(int node = 0 ; node < 64 ; node++)
        make_node(node + 9, subtree);

    for(int node = 0 ; node < 8 ; node++)
        make_node(node + 1, subtree);

    make_node(0, subtree);
}

/**
 * @brief Update a leaf hash.
 *
 * @param leaf The leaf to get the hash of.
 */
static void update_leaf(int leaf)
{
    uint64_t *tree = get_tree(leaf);
    uint64_t *place = NNODES + tree + (leaf % NLEAVES);
    *place = memory_list_hash(leaf);
}

/**
 * @brief Update a leaf hash and all its ancestors.
 *
 * @param leaf The leaf to update.
 */
static void update_branch(int leaf)
{
    uint64_t *tree = get_tree(leaf);
    leaf %= NLEAVES;
    leaf += NNODES;

    for(int i = 0 ; i < 3 ; i++) {
        leaf--;
        leaf /= 8;
        make_node(leaf, tree);
    }
}

/**
 * @brief Insert a message in the tree and update its ancestors.
 *
 * @param b A pointer to the bucket to insert.
 *
 * @return the address of the newly inserted bucket, or MEM_FULL if
 * if failed, or HAS_BUCKET if the bucket is already in memory.
 */
static uint16_t place_message(struct Bucket *b)
{
    // Use the first 10 bits of the ID to place the message in the tree.
    uint16_t id = small_id(b->type.id);

    uint16_t result = memory_insert_bucket(b);
    // Insert the bucket in the appropriate list.
    if(result == MEM_FULL)
        return MEM_FULL;
    if(result == HAS_BUCKET)
        return HAS_BUCKET;

    update_leaf(id);

    return id;
}

// The structure of the tree is the following: there is a root node with
// two sons. Each of these can be viewed as the root of a three-levels
// 8-sons tree, with 512 leaves each.
void build_tree(void)
{
    chMtxLock(&tree_mtx);
    for(int i = 0 ; i < TREE_SIZE ; i++)
        update_leaf(i);

    make_subtree(ltree);
    make_subtree(rtree);
    chMtxUnlock();
}

uint16_t tree_insert(struct Bucket *b)
{
    chMtxLock(&tree_mtx);
    uint16_t id = place_message(b);
    if(id == MEM_FULL) {
        chMtxUnlock();
        return MEM_FULL;
    }
    update_branch(id);
    chMtxUnlock();
    return id;
}

#ifdef __MEMTESTS__
uint64_t* get_l(void)
{
    return ltree;
}

uint64_t* get_r(void)
{
    return rtree;
}
#endif //__MEMTESTS__

void tree_clean(void)
{
    chMtxLock(&tree_mtx);
    int n = 0;
    ltree[n] = 2936206172805255438ULL;
#ifndef __SMALL_TREE__
    rtree[0] = 0;
    rtree[n] = 2936206172805255438ULL;
#endif // __SMALL_TREE__

    n++;
    for(int i = 0 ; i < 8 ; i++, n++) {
        ltree[n] = 8154021761649030031ULL;
#ifndef __SMALL_TREE__
        rtree[n] = 8154021761649030031ULL;
#endif // __SMALL_TREE__
    }
    for(int i = 0 ; i < 64 ; i++, n++) {
        ltree[n] = 14472265652744616578ULL;
#ifndef __SMALL_TREE__
        rtree[n] = 14472265652744616578ULL;
#endif // __SMALL_TREE__
    }
    for(int i = 0 ; i < 512 ; i++, n++) {
        ltree[n] = 0;
#ifndef __SMALL_TREE__
        rtree[n] = 0;
#endif // __SMALL_TREE__
    }
    assert(n == 585);
    chMtxUnlock();
}

void get_hash_and_sons(uint8_t n, uint64_t **f, uint64_t **s)
{
    chMtxLock(&tree_mtx);
    uint64_t *t = n & (1 << 7) ? rtree : ltree;
    n &= ~(1 << 7);
    *f = t + n;
    *s = t + 8*n + 1;
    chMtxUnlock();
}

void tree_get_roots(void *buf)
{
    chMtxLock(&tree_mtx);
    memcpy((uint8_t *) buf, ltree, 8);
#ifndef __SMALL_TREE__
    memcpy(((uint8_t *) buf) + 8, rtree, 8);
#else
    memset(((uint8_t *) buf) + 8, 0, 8);
#endif // __SMALL_TREE__
    chMtxUnlock();
}

uint64_t get_leaf_hash(uint16_t leaf)
{
    chMtxLock(&tree_mtx);
    uint64_t *tree = get_tree(leaf);
    leaf %= NLEAVES;
    chMtxUnlock();
    return tree[NNODES + leaf];
}

int get_list(uint16_t leaf, uint64_t *buffer)
{
    chMtxLock(&tree_mtx);
    int length = 0;
    uint16_t address = read_list_address(leaf);

    if (address == NO_BUCKET) {
        chMtxUnlock();
        return 0;
    }

    while (address != NO_BUCKET) {
        buffer[length] = BUCKET_READ_FIELD(address, type.id, 64);
        address = BUCKET_READ_FIELD(address, next_id, 16);
        length++;
    }

    chMtxUnlock();
    return length;
}

uint8_t cmp_lists(uint16_t leaf, void *buf, uint8_t length)
{
    chMtxLock(&tree_mtx);
    uint16_t b = memory_get_ids_head(leaf);
    if (b == NO_BUCKET) {
        chMtxUnlock();
        return 0;
    }
    uint64_t hash = BUCKET_READ_FIELD(b, type.id, 64);
    uint8_t nb_to_send = 0;

    unsigned int i = 0;
    while(b != NO_NEXT && i < length)
        if(hash == ((uint64_t *) buf)[i+1]) {
            b = BUCKET_READ_FIELD(b, next_id, 16);
            i++;
        } else if(BUCKET_READ_FIELD(b, type.id, 64) < ((uint64_t *) buf)[i+1]){
            i++;
        } else {
            ((uint16_t *) buf)[nb_to_send++] = b;
            b = BUCKET_READ_FIELD(b, next_id, 16);
        }

    while(b != NO_NEXT) {
        ((uint16_t *) buf)[nb_to_send++] = b;
        b = BUCKET_READ_FIELD(b, next_id, 16);
    }

    chMtxUnlock();
    return nb_to_send;
}

int tree_has_message(uint64_t id)
{
    chMtxLock(&tree_mtx);
    uint16_t leaf = small_id(id);
    uint16_t position = memory_get_ids_head(leaf);
    if(position == NO_BUCKET) {
        chMtxUnlock();
        return HAS_NOT;
    }
    uint64_t hash = BUCKET_READ_FIELD(position, type.id, 64);

    while((position != NO_BUCKET) && (id > hash)) {
        hash = BUCKET_READ_FIELD(position, type.id, 64);
        position = BUCKET_READ_FIELD(position, next_id, 16);
    }

    chMtxUnlock();

    if(id == hash)
        return HAS;
    return HAS_NOT;
}

void tree_reset(void)
{
    chMtxLock(&tree_mtx);
    memory_clean();
    chMtxUnlock();
    tree_clean();
}

/**
 * @brief Given an address and a place in memory, provide the
 * @brief next message meant for this address.
 *
 * @param id The destination address.
 * @param start the place in memory.
 *
 * @return The address of the next bucket.
 */
static uint16_t id_next_message(uint16_t id, uint16_t start)
{
    for (int i = start ; i < 1024 ; i++)
        if (BUCKET_READ_FIELD(i, destination_address, 16) == id)
            return i;
    return END_REACHED;
}

uint16_t next_message(uint16_t id, uint16_t start, struct Bucket *buf)
{
    uint16_t address = id_next_message(id, start);
    if (address == END_REACHED)
        buf->state = 0;
    else {
        read_bucket(address, buf);
        buf->state |= 1;
    }
    return address;
}
