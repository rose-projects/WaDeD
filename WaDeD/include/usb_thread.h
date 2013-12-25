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
 * @file  usb_thread.h
 * @brief A thread to interract with the client.
 */

#ifndef __USB_THREAD_H__
#define __USB_THREAD_H__

#include "ch.h"
#include "hash.h"
#include "tree.h"
#include "timestamp.h"

msg_t usb_thread(void *arg);

void usb_thread_init(void);

#endif // __USB_THREAD_H__
