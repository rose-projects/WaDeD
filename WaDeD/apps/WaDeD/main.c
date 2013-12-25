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

#include <stdlib.h>
#include "ch.h"
#include "dash7.h"
#include "hal.h"
#include "waded_usb.h"
#include "sx1231.h"
#include "dio.h"
#include "jungle.h"
#include "fifo.h"
#include "tree.h"
#include "memory.h"
#include "bucket.h"
#include "usb_thread.h"

#define NO_LED

#ifndef NO_LED
#include "led.h"
#endif

#define NO_USB

#ifdef NO_USB
#define usb_puts(...)
#endif

#define MAX_SIZE_MSG 164

#ifdef RADIO_TEST
static char tx_buffer[MAX_SIZE_MSG];
#endif

int DEVICE_ID = 2;
static char rx_buffer[MAX_SIZE_MSG];

#define RX_MIN_TIMEOUT  MS2ST(500)
#define RX_RAND_MAX     MS2ST(2000)
#define TX_TIMEOUT      TIME_IMMEDIATE

static inline uint32_t rand32(uint32_t max)
{
    return (uint32_t) (max * ((float) rand() / (float) (RAND_MAX)));
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
#ifndef NO_LED
    led_init();
#endif
    chThdSleepSeconds(3);

    tree_reset();
    usb_printf("GO!\n");

    srand((uint32_t) chTimeNow());
    int state = TX_SUCCESS;
    ssize_t size = 0;

    usb_thread_init();

    for(;;) {
        usb_puts("Receiving\n");
        size = receive_packet(rx_buffer,
                              sizeof rx_buffer,
                              RX_MIN_TIMEOUT + rand32(RX_RAND_MAX));

        if (size == TIMEOUT) {
            usb_puts("RX Timeout\n");
        } else if (size == RECEIVE_CRC_ERROR) {
            usb_puts("RX Timeout\n");
        } else if (size == RECEIVE_TOO_LONG) {
            usb_puts("ERROR Too long\n");
        } else {
            usb_printf("Message received type: %d - %d\n", rx_buffer[0],
                            rx_buffer[2]);
            handle_packet(rx_buffer);
            extern uint16_t memory_counter;
            usb_printf("memory_counter = %d\n", memory_counter);
        }

        // TODO: Check if the handle packet running time is enough
        // or if we really need this pause.
        chThdSleepMilliseconds(10);

        // Transmit the current message in the tx_buffer if the previous
        // transmission attempt failed.
        // If the previous transmission was a success, we call fifo_pop to
        // have the next one. If their is a new message in tx_buffer, fifo_pop
        // return 1.
        if (state == CHANNEL_BUSY || (state == TX_SUCCESS && fifo_pop())) {
#ifndef NO_USB
            usb_puts("Transmitting\n");
#endif
            state = send_packet(tx_buffer, tx_buffer[1] + 2, TIME_IMMEDIATE);
#ifndef NO_USB
            if (state == CHANNEL_BUSY)
                usb_puts("TX busy\n");
            else
                usb_printf("Message transmitted type: %d - %d\n",
                                tx_buffer[0], tx_buffer[2]);
#endif
        }
#ifndef NO_LED
        led_toggle();
#endif
    }
}
