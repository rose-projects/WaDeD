/*
 * WaDeD - Short messages mesh network
 *
 * Copyright (C) 2013 WaDeD-ROSE <waded-rose@googlegroups.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "fram_test.h"
#include "ch.h"
#include "dash7.h"
#include "hal.h"
#include "led.h"
#include "waded_usb.h"
#include "sx1231.h"
#include "memtests.h"

int main(void)
{
    halInit();
    chSysInit();
    fram_init();
    sx_init();
    led_init();
    led_thread();

    usb_init();
    chThdSleepSeconds(5 );

    usb_printf("This is the WaDeD functionnality check.\n");
    int success = 1;

    fram_id();
    success &= fram_test();

#ifdef EXTENDED
    memory_tests(15);
#endif

    if (success)
        usb_printf("This WaDeD is fully functionning.\n");
    else
        usb_printf("There are some problems with this WaDeD.\n");
}
