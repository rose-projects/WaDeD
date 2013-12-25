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
 * @file  hash.c
 * @brief Functions to hash a buffer with sha1.
 */

#include "hash.h"
#include "sha1.h"

/**
 * @brief Hash with SHA1.
 *
 * @param value  What to hash.
 * @param size   The size of what to hash.
 * @param buffer The output buffer.
 *
 * @return Sha Error Code (see sha1 file).
 */
static int sha1(const uint8_t *value, int size, uint8_t *buffer)
{
    SHA1Context sha;
    SHA1Reset(&sha);
    SHA1Input(&sha, value, size);
    return SHA1Result(&sha, buffer);
}

uint64_t hash(const void *buffer, size_t size)
{
    // Compute the hash.
    uint8_t h [8];
    sha1((uint8_t *) buffer, size, h);

    // Convert into an uint64_t.
    uint64_t ret = 0;
    for (int i = 0; i < 8; i++) {
        ret <<= 8;
        ret += h[i];
    }

    return ret;
}
