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
 * @file  jungle.h
 * @brief Implement the jungle protocol.
 */

#ifndef __JUNGLE_H__
#define __JUNGLE_H__

#include <stdint.h>

#define NODE 0
#define ROOT 1
#define LEAF 2
#define MESSAGE 3

/**
 * @brief Handles a recieved packet, chooses what has to be sent in response.
 *
 * @param buf Pointer towards a received buffer.
 */
void handle_packet(const void *buf);

#endif // __JUNGLE_H__
