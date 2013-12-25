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
 * @file  bucket.h
 * @brief Declare the struct Bucket, used for message storage.
 */

#ifndef __BUCKET_H__
#define __BUCKET_H__

/**
 * @brief A memory bucket for storage of a message and its metadata.
 */
struct Bucket {
    union {
        struct {
            uint16_t next;
            uint8_t first;
        } empty;
        uint64_t id;
    } type;
    uint32_t emission_date; // Emission timestamp
    uint32_t expiration_date; // Expiration timestamp
    uint16_t source_address;
    uint16_t destination_address;
    uint16_t next_timestamp;
    uint16_t next_id; // ID of the next packet
    uint8_t  hop_limit;
    uint8_t  type_id;

    // The state field holds several 1-bit informations:
    // bit 0 is set when the bucket is used.
    // bit 1 is set when the bucket is the first bucket of timestamps.
    // bit 2 is set when the bucket is the first bucket of its ID list.

    uint8_t  state;

    uint8_t  message [141];
};

#endif // __BUCKET_H__
