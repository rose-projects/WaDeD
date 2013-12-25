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
 * @file  timestamp.h
 * @brief Functions about timestamps.
 */

#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>

uint32_t get_timestamp(void);
uint32_t set_timestamp(uint16_t year,
                       uint16_t month,
                       uint16_t day,
                       uint16_t hour,
                       uint16_t minute,
                       uint16_t second);

#endif // _TIMESTAMP_H__
