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
 * @file  dash7.h
 * @brief Give all the functions to configure dash7 on the SX1231.
 * */
#ifndef _DASH7_H
#define _DASH7_H

#include <stdint.h>
#include "ch.h"

// Configuration definition
#define VALUE_RSSI_THRESH 200

/**
 * @brief   Initialise the X1231, with the Dash7 options.
 *
 * @default The channel is the channel BASE. The mode is the FOREGROUND_MODE,
 * the fifo threshold is set at VALUE_FIFO_THRESH (defined in sx1231.h) and the
 * rssi threshold is set at VALUE_RSSI_THRESH (defind above).
 */
void dash7_init(void);

/**
 *
 */
#define BASE_CHANNEL    0x07
#define LEGACY_CHANNEL  0x01
#define NORMAL_CHANNEL  0x10
#define HI_RATE_CHANNEL 0x27
#define BLINK_CHANNEL   0x32

/**
 * @brief Select the designed channel.
 *
 * @param channel    See above for the different channels.
 */
void select_channel(uint8_t channel);

#define BACKGROUND_MODE 0xE6D0
#define FOREGROUND_MODE 0x0B67

/**
 * @brief Select the packet mode
 *
 * @param mode     The packet mode. Background or foreground mode.
 */
void select_packet_mode(uint16_t mode);

#endif // _DASH7_H
