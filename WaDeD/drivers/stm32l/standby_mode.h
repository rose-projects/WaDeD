/*
 *  WaDeD - Short messages mesh network
 *
 *  Copyright (C) 2013 ROSE <waded-rose@googlegroups.com>
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
 * @file  standby_mode.h
 * @brief A function making the STM32 sleep.
 */

#ifndef __STANBY_MODE_H__
#define __STANBY_MODE_H__

#include "stdint.h"

/**
 * @brief Put the STM32L in sleep for certain times.
 *
 * @param sec    Time during which the STM32 will be asleep.
 */
void deep_sleep(uint16_t sec);

#endif // __STANBY_MODE_H__
