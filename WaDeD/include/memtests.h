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
 * @file  memtests.h
 * @brief Tests to check if memory functions are well functionning.
 */

#ifndef __MEMTESTS_H__
#define __MEMTESTS_H__

uint32_t rand32(uint32_t max);
void memory_tests(int rand_seed); // Takes about 5 minutes on WaDeD
void random_insertion_reading(uint32_t n);
void upper_boundary_tests(uint32_t n);
void insert_read_packets(int n);
void insert_erase_packets(int n);
void check_timestamps(void);
void test_id_list(void);
void hash_a_small_list(void);
void init_fram_randomly(int n);

#endif // __MEMTESTS_H__
