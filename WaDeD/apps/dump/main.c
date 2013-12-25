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
 * @file main.c
 *
 * @brief Dumper, which will print all communication it gets on the screen.
 */
#include <stdlib.h>
#include "sleep_until.h"
#include "ch.h"
#include "dash7.h"
#include "hal.h"
//#include "led.h"
#include "waded_usb.h"
#include "sx1231.h"
#include "dio.h"
#include "jungle.h"
#include "fifo.h"
#include "tree.h"
#include "memory.h"
#include "bucket.h"
#include "usb_thread.h"

#define MAX_SIZE_MSG 164

#ifdef RADIO_TEST
static char tx_buffer[MAX_SIZE_MSG];
#endif

static char rx_buffer[MAX_SIZE_MSG];

#define RX_MIN_TIMEOUT  MS2ST(500)
#define RX_RAND_MAX     MS2ST(2000)
#define TX_TIMEOUT      TIME_IMMEDIATE

#define NO_USB

static inline uint32_t rand32(uint32_t max)
{
    return (uint32_t) (max * ((float) rand() / (float) (RAND_MAX)));
}

static void dump_node(const void *buf)
{
    usb_printf("NODE %d\n"
            "sons: %x %x %x %x %x %x %x %x",
            ((uint8_t *) buf)[2] & 0x0F,
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[1],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[2],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[3],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[4],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[5],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[6],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[7],
            (uint32_t) ((uint64_t *) ((uint8_t *) buf + 3))[8]);
}

static void dump_root(const void *buf)
{
#ifndef __SMALL_TREE__
    usb_printf("ROOT\nlroot: %x rroot: %x",
            (uint32_t) ((uint64_t *) buf)[0],
            (uint32_t) ((uint64_t *) buf)[1]);
#else
    usb_printf("ROOT\nroot: %x", (uint32_t) ((uint64_t *) buf)[0]);
#endif // __SMALL_TREE__
}

static void dump_list(const void *buf)
{
    uint8_t lsize = (uint8_t) (((uint16_t *) buf)[0] & 0x3F);
    usb_printf("LIST %d\n", (((uint16_t *) buf)[0] >> 6));
    usb_printf("size: %d\n", lsize);

    for (int i = 0 ; i < lsize + 1 ; i++)
        usb_printf("N%d: %x ", i, (uint32_t)
                        ((uint64_t *) (((uint8_t *) buf) + 2))[i]);
    // This is definitely the worst single line of code I have ever written.
}

static void dump_message(const void *buf)
{
    usb_printf("MESSAGE\n%s", buf);
}

void dump_packet(const void *buf)
{
    uint8_t version = ((uint8_t *) buf)[0] >> 4;
    uint8_t type    = ((uint8_t *) buf)[0] & 0x0F;
    uint8_t length  = ((uint8_t *) buf)[1];

    usb_printf(
#ifndef __TAG_MODE__
            "version %d "
#else
            "device %d "
#endif
            "length %d "
            "type ",
            version,
            length);

    switch(type) {
        case(NODE):
            dump_node(((uint8_t *) buf));
            break;
        case(ROOT):
            dump_root(((uint8_t *) buf) + 2);
            break;
        case(LEAF):
            dump_list(((uint8_t *) buf) + 2);
            break;
        case(MESSAGE):
            dump_message(((uint8_t *) buf) + 2);
            break;
    }
}

int main(void)
{
    halInit();
    chSysInit();
    fram_init();
    usb_init();
    sx_init();
    dash7_init();
    dio_init();
    //    led_init();
    chThdSleepSeconds(3);

    srand((uint32_t) chTimeNow());
    ssize_t size = 0;

    usb_thread_init();

    for(;;) {
        usb_puts("Receiving\n");
        // Receiving
        size = receive_packet(rx_buffer,
                sizeof rx_buffer,
                RX_MIN_TIMEOUT + rand32(RX_RAND_MAX));

        if (size == TIMEOUT)
            usb_puts("RX Timeout");
        else if (size == RECEIVE_CRC_ERROR)
            usb_puts("RX Timeout");
        else if (size == RECEIVE_TOO_LONG)
            usb_puts("ERROR Too long");
        else
            dump_packet(rx_buffer);
        usb_puts("\n\n");

        chThdSleepMilliseconds(10);
    }
}
