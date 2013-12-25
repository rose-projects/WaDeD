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
 * @file  waded_usb.h
 * @brief Configuration and functions for the USB connexion.
 */

#ifndef __USB_H__
#define __USB_H__

#include <stdio.h>
#include <string.h>

#include "ch.h"
#include "hal.h"
#include "test.h"

#include "usb_cdc.h"
#include "shell.h"
#include "chprintf.h"

#include "fram.h"

/**
 * @brief Print a string on the USB.
 *
 * @param buffer    The buffer to print on the USB.
 */
void usb_puts(const char * buffer);

/**
 * @brief Print a character on the USB.
 *
 * @param tosend    The character to send.
 */
void usb_putc(const char tosend);

/**
 * @brief Printf to print whatever we want on the USB.
 */
#ifndef NO_USB
#define usb_printf(...) chprintf((BaseSequentialStream *) &SDU1, __VA_ARGS__)
#else
#define usb_printf(...)
#endif

/**
 * @brief  Get a character from the USB.
 *
 * @return Returns a character received form the USB.
 */
char usb_get(void);

/**
 * @brief Get data from the USB.
 *
 * @param buffer    The buffer where the data gets will be stocked.
 * @param size      Size of the buffer.
 */
void usb_read(char * buffer, size_t size);

/**
 * @brief Initialise the USB driver.
 */
void usb_init(void);

// Use this for direct stream access
extern SerialUSBDriver SDU1;

// These functions are only used as a way to ensure compatibility with
// the alpha version. Their use is deprecated.
static inline void usb_send_str(char *str){
    usb_puts(str);
}

static inline void usb_send_line(char *str){
    usb_send_str(str);
    chSequentialStreamWrite(&SDU1,(uint8_t*)"\n\r",2);
}

static inline void usb_send_data(uint8_t *data, uint16_t size){
    chSequentialStreamWrite(&SDU1,data,size);
}

static inline uint16_t usb_receive_data(uint8_t *data, uint16_t max_size){
    return chSequentialStreamRead(&SDU1,data,max_size);
}

static inline uint16_t usb_getc(void){
    return chSequentialStreamGet(&SDU1);
}

static inline void usb_start(void){
    usb_init();
}

#endif
