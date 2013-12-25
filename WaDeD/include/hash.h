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
 * @file  hash.h
 * @brief Functions to hash a buffer with SHA1.
 */

#ifndef __HASH_H__
#define __HASH_H__

#include <stdint.h>
#include <stdlib.h>

/**
 * @brief Hash the content of a buffer with SHA1.
 *
 * @param buffer The buffer to be hashed.
 * @param size   The buffer size.
 *
 * @return The first 64 bits of the SHA1 hash.
 */
uint64_t hash(const void *buffer, size_t size);

#endif // __HASH_H__
