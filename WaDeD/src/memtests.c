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

#define __MEMTESTS__
#include "memory.h"
#include "tree.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#ifdef LAB_BOARD
#define NPAGES 58
#define PSIZE  1024
#define FRAM_SIZE (NPAGES * PSIZE)
#else
#define FRAM_SIZE 262144
#endif //LAB_BOARD

#include "bucket.h"
#include "waded_usb.h"
#define print(...) usb_printf(__VA_ARGS__)

uint32_t rand32(uint32_t max)
{
    float r = max * ((float)(rand()) / (float)(RAND_MAX));
    return (uint32_t) r;
}

void random_insertion_reading(uint32_t n)
{
    unsigned int c = 0;

    for(unsigned int i = 0 ; i < n ; i++) {
        uint32_t address = rand32(FRAM_SIZE - 4);
        uint32_t wvalue  = rand32(0xFFFFFFFF);

        fram_write32(address, wvalue);
        uint32_t rvalue  = fram_read32(address);

        if(rvalue == wvalue) c++;
        else {
            print("Error in copying. Aborting.\n");
            print("Wrote 0x%x at address 0x%x but read 0x%x.\n",
                  wvalue, address, rvalue);
            break;
        }
    }

    if(c==n)
        print("FRAM pass %d random insertion and reading tests.\n", n);
}

void upper_boundary_tests(uint32_t n)
{
    unsigned int c = 0;

    for(unsigned int i = 0 ; i < n ; i++) {
        static uint8_t buffer_out [0xFF];
        static uint8_t buffer_in  [0xFF];

        uint8_t nbytes = rand32(0xFE) + 1;

        for(int j = 0 ; j < nbytes ; j++) {
            buffer_out[j] = rand32(0xFF);
        }

        fram_write(FRAM_SIZE - nbytes, buffer_out, nbytes);
        fram_read(FRAM_SIZE - nbytes, buffer_in, nbytes);

        if(~memcmp(buffer_in, buffer_out, nbytes)) c++;
        else {
            print("Error in copying %d bytes at test %d."
                  "Aborting.\n", nbytes, i+1);

            for(int j = 0 ; j < nbytes ; j++)
                if(buffer_in[j] != buffer_out[j])
                    print("0x%x 0x%x\n",
                          buffer_out[j],
                          buffer_in[j]);

            break;
        }
    }
    if(c==n)
        print("FRAM pass %d upper boundary tests.\n", n);

}

static void random_bucket(struct Bucket* buf)
{
    buf->type.id = ((uint64_t) rand32(0xFFFFFFFF) << 32)
        + rand32(0xFFFFFFFF);
    buf->source_address = (uint16_t) rand32(0xFFFF);
    buf->destination_address = (uint16_t) rand32(0xFFFF);
    buf->emission_date = rand32(0xFFFFFFFF);
    buf->hop_limit = 0;
    buf->type_id = 0;
    buf->expiration_date = rand32(0xFFFFFFFF);
    buf->next_id = 0;
    buf->state = 0;

    int size = rand32(140);
    int i;

    for(i = 0 ; i < size ; i++)
        buf->message[i] = (uint8_t) 'a' + rand32(25);

    for(; i < 141 ; i++)
        buf->message[i] = 0;
}

void insert_read_packets(int n)
{
    memory_clean();
    int c = 0;
    static struct Bucket buf;
    static struct Bucket buf_r;
    uint16_t address;

    for(int i = 0 ; i < n ; i++) {
        random_bucket(&buf);
        address = memory_insert_bucket(&buf);
        if(address == MEM_FULL) {
            print("Tried to insert message but memory "
                  "was full.\n");
            return;
        }

        read_bucket(address, &buf_r);

        if(!memcmp(&buf.type.id, &buf_r.type.id, sizeof(uint64_t)))
            c++;
    }

    if(c == n)
        print("Successfully inserted and read %d buckets.\n", n);
    else
        print("Error with insertion/reading.\n");
}

void insert_erase_packets(int n)
{
    memory_clean();
    int c = 0;
    static uint16_t addresses [100];
    struct Bucket buf;
    for(int i = 0 ; i < n ; i++) {
        for(int j = 0 ; j < 100 ; j++) {
            random_bucket(&buf);
            addresses[j] = memory_insert_bucket(&buf);
            if(addresses[j] == MEM_FULL) {
                print("Tried to insert message but memory was"
                      "full.\n");
                return;
            }
        }

        for(int j = 0 ; j < 100 ; j++)
            memory_erase_bucket(addresses[j]);

        c++;
    }

    if(c == n)
        print("Inserted and removed %d times 100 packets.\n", n);
}

static void sort_by_expiration(struct Bucket *buf, int size)
{
    for(int i = 1 ; i < size ; i++) {
        struct Bucket tmp = buf[i];
        int j = i;
        while(j > 0 &&
              buf[j - 1].expiration_date > tmp.expiration_date) {
            buf[j] = buf[j - 1];
            j--;
        }

        buf[j] = tmp;
    }
}

static struct Bucket bucket_buf [50];

void check_timestamps(void)
{
    memory_clean();

    for(int i = 0 ; i < 50 ; i++) {
        random_bucket(&bucket_buf[i]);
        memory_insert_bucket(&bucket_buf[i]);
    }

    sort_by_expiration(bucket_buf, 50);

    int c = 0;
    for(int i = 0 ; i < 50 ; i++) {
        static struct Bucket buf_r;
        uint16_t head = memory_get_timestamps_head();

        read_bucket(head, &buf_r);

        memory_erase_bucket(head);

        if(buf_r.expiration_date == bucket_buf[i].expiration_date)
            c++;
    }

    if(c == 50)
        print("The timestamps are correctly ordered for %d buckets.\n",
              50);
}

static void sort_by_id(struct Bucket *buf, int size)
{
    for(int i = 1 ; i < size ; i++) {
        struct Bucket tmp = buf[i];
        int j = i;
        while(j > 0 && buf[j - 1].type.id > tmp.type.id) {
            buf[j] = buf[j - 1];
            j--;
        }

        buf[j] = tmp;
    }
}

void test_id_list(void)
{
    memory_clean();

    for(int i = 0 ; i < 50 ; i++) {
        random_bucket(&bucket_buf[i]);
        bucket_buf[i].type.id = (bucket_buf[0].type.id
                                 & ~((uint64_t) 0xFFFF)) + 24 - 4*i;
        memory_insert_bucket(&bucket_buf[i]);
    }
    uint16_t chosen_id = small_id(bucket_buf[0].type.id);

    sort_by_id(bucket_buf, 50);

    static uint64_t ids [50];
    int c = 0;
    for(int i = 0 ; i < 50 ; i++) {
        uint16_t address = memory_get_ids_head(chosen_id);
        ids[i] = BUCKET_READ_FIELD(address,
                                   type.id, 64);
        memory_erase_bucket(memory_get_ids_head(chosen_id));

        if(bucket_buf[i].type.id == ids[i])
            c++;
    }

    if(c == 50)
        print("Inserting %d elements in the same list OK.\n", 50);
}

void hash_a_small_list(void)
{
    int c = 0;
    memory_clean();
    uint64_t hash1, hash2;

    for(int j = 0 ; j < 100 ; j++) {
        memory_clean();

        for(int i = 0 ; i < 50 ; i++) {
            random_bucket(&bucket_buf[i]);
            bucket_buf[i].type.id = (bucket_buf[0].type.id
                                     & ~((uint64_t) 0xFF))
                + 24 - 4*i;
            memory_insert_bucket(&bucket_buf[i]);
        }
        uint16_t chosen_id = small_id(bucket_buf[0].type.id);

        hash1 = memory_list_hash(chosen_id);

        memory_clean();

        for(int i = 50 - 1; i >= 0; i--)
            memory_insert_bucket(&bucket_buf[i]);

        hash2 = memory_list_hash(chosen_id);

        if(hash1 == hash2)
            c++;
    }
    if(c == 50)
        print("Hashes are the same.\n");
}

void init_fram_randomly(int n)
{
    int c = 0;
    for(int k = 0 ; k < n ; k++) {
        tree_clean();
        memory_clean();
        struct Bucket buf [100];
        uint64_t r1 [512];
        uint64_t l1 [512];
        uint64_t r2 [512];
        uint64_t l2 [512];

        for(int i = 0 ; i < 100 ; i++) {
            random_bucket(&buf[i]);
            memory_insert_bucket(&buf[i]);
        }
        // Now the FRAM contains random messages. Let's try to init a tree
        // with this FRAM.

        build_tree();
        for(int i = 0 ; i < 512 ; i++) {
            r1[i] = get_r()[i];
            l1[i] = get_l()[i];
        }

        memory_clean();
        tree_clean();

        for(int i = 0 ; i < 100 ; i++)
            tree_insert(&buf[i]);
        for(int i = 0 ; i < 512 ; i++) {
            r2[i] = get_r()[i];
            l2[i] = get_l()[i];
        }
        build_tree();

        if(l1[0] == l2[0] && r1[0] == r2[0])
            c++;
    }
    if(c == n)
        print("Top hashes match.\n");
}

void memory_tests(int rand_seed)
{
    srand(rand_seed);
    random_insertion_reading(10000);
    upper_boundary_tests(10000);
    insert_read_packets(100);
    insert_erase_packets(100);
    check_timestamps();
    test_id_list();
    hash_a_small_list();
}
