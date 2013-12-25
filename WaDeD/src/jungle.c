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
 * @file  jungle.c
 * @brief Implement the jungle protocol.
 */

#include <string.h>

#include "jungle.h"
#include "tree.h"
#include "fifo.h"
#include "client_cmd.h"

#define unless(x) if(!(x))

// #define __QUIET__

/**
 * @brief Place messages in the fifo giving information about the sons of a
 * node.
 *
 * @param n    The node.
 * @param diff The sons that differ.
 */
static void do_node(uint8_t n, uint8_t diff)
{
#ifdef __SMALL_TREE__
    if(n & (1 << 7))
        return;
#endif // __SMALL_TREE__
    uint8_t node = n & ~(1 << 7); // The node number in the tree.

    if(node < 9) // The node's sons are nodes.
        for(int i = 0; i < 8; i++) {
            if(diff & (1 << i)) {
                uint8_t p = 8 * node + i + 1; // The son number in the tree.
                p += n & (1 << 7); // Add the information about which tree.
                fifo_push(p, NODE);
#ifdef __QUIET__
                break;
#endif
            }
        }
    else // The node's sons are leaves.
        for(int i = 0; i < 8; i++) {
            if(diff & (1 << i)) {
                uint16_t p = 8 * node + i + 1; // The son number in the tree.
                p -= 73; // The leaf number in the tree.
                p += (n & (1 << 7)) ? 512 : 0; // The global leaf number.
                fifo_push(p, LEAF);
#ifdef __QUIET__
                break;
#endif
            }
        }
}

/**
 * @brief Handle the reception of a NODE message.
 *
 * @param buf The input message.
 */
static void handle_node(const void *buf)
{
    // Get the information about this node in our memory.
    uint8_t n = ((uint8_t *) buf)[0];
    uint64_t *top;
    uint64_t *sons;
    get_hash_and_sons(n, &top, &sons);

    // Compare it with the input to know if we have something to do.
    unless(memcmp(top, ((uint8_t *) buf) + 1, 8))
        return;

    // Determine the differences in the sons' hashes.
    uint8_t diff = 0;
    for(int i = 0; i < 8; i++)
        if(memcmp(sons + i, ((uint8_t *) buf) + 9 + 8 * i, 8))
            diff |= (1 << i);

    // If there are some differences, we call the function that takes action.
    if(diff)
        do_node(n, diff);
}

/**
 * @brief Handle the reception of a ROOT message.
 *
 * @param buf The input message.
 */
static void handle_root(const void *buf)
{
    // Get our roots.
    uint8_t h [16];
    tree_get_roots(h);

    // Compare them, and in case of a difference, send it.
    if(memcmp(h, (uint8_t *) buf, 8))
        fifo_push((0 << 7), NODE);
#ifndef __SMALL_TREE__
    if(memcmp(h + 8,  ((uint8_t *) buf) + 8, 8))
        fifo_push((1 << 7), NODE);
#endif // __SMALL_TREE__
}

/**
 * @brief Handle the reception of a LIST message.
 *
 * @param buf The input message.
 */
static void handle_list(const void *buf)
{
    // Get the information about this list in our memory.
    uint16_t leaf      = ((uint16_t *) buf)[0] >> 6;
    uint8_t  list_size = (uint8_t) (((uint16_t *) buf)[0] & 0x3F);
    uint64_t top       = get_leaf_hash(leaf);

    unless(memcmp(&top, ((uint8_t *) buf) + 2, 8)) // We have the same list.
        return;

    // Determine how many leaves we will send.
    uint8_t to_send = cmp_lists(leaf, ((uint8_t *) buf) + 2, list_size);

    // If we have no leaf to send, we send our list, so the other will send the
    // difference.
    unless(to_send)
        fifo_push(leaf, LEAF);

    // If we have some messages to send, send them.
    for(int i = 0; i < to_send; i++)
        fifo_push((((uint16_t *) buf) + 1)[i], MESSAGE);
}

/**
 * @brief Handle the reception of a MESSAGE message.
 *
 * @param buf    The intput message.
 * @param length Its size.
 */
void handle_message(const void *buf, uint8_t length)
{
    // Build a bucket from the buffer.
    static struct Bucket b;
    memcpy(&b.type.id, buf, 8);
    if(tree_has_message(b.type.id))
        return;
    b.emission_date       = ((uint32_t *) buf)[2];
    b.expiration_date     = ((uint32_t *) buf)[3];
    b.source_address      = ((uint16_t *) buf)[8];
    b.destination_address = ((uint16_t *) buf)[9];

    // Set unused fields at 0
    b.hop_limit = 0;
    b.type_id   = 0;

    // Copy message.
    for(int i = 20; i < length; i++)
        b.message[i - 20] = ((uint8_t *) buf)[i];
    b.message[length - 20] = 0;

    // Add the message in memory.
    tree_insert(&b);

#ifndef __SIMU__
    // Send message to user.
    send_usr_message(&b);
#endif
}

void handle_packet(const void *buf)
{
    uint8_t version = ((uint8_t *) buf)[0] >> 4;
#ifndef __TAG_MODE__
    if (version == 0) {
#else
    (void) version;
#endif
        uint8_t type    = ((uint8_t *) buf)[0] & 0x0F;
        uint8_t length  = ((uint8_t *) buf)[1];

        switch(type) {
            case(NODE):
                handle_node(((uint8_t *) buf) + 2);
                break;
            case(ROOT):
                handle_root(((uint8_t *) buf) + 2);
                break;
            case(LEAF):
                handle_list(((uint8_t *) buf) + 2);
                break;
            case(MESSAGE):
                handle_message(((uint8_t *) buf) + 2, length);
                break;
        }
#ifndef __TAG_MODE__
    }
#endif
}
