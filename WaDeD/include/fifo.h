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

/*
 * @file  fifo.h
 * @brief Handle a fifo to determine to future messages to send.
 */

#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>

#ifndef __SIMU__
#include "ch.h"
#include "hal.h"
#endif

#define FIFO_MAXSIZE 20

/**
 * @brief Places in fifo the information about a packet to send.
 *
 * @param arg  The data allowing to construct the packet.
 * @param type The type of packet to be sent.
 */
#ifndef __SIMU__
//USE_MEMORY
extern uint8_t tx_buffer[164];
#endif

/**
 * @brief Push the command for a message to send in the FIFO.
 *
 * @param arg    Arguments related to tthe type of the message.
 * @param type   Type of the messsage.
 */
void fifo_push(uint16_t arg, uint8_t type);

/**
 * @brief Pop a message from the fifo into tx_buffer.
 */
int fifo_pop(void);

#endif // __FIFO_H__
