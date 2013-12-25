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

#include "fram.h"
#include "fram_test.h"

#include "waded_usb.h" // For test

void fram_id(void)
{
    // Get and put on usb the FRAM id
    static uint8_t fram_id[9];
    fram_rdid(fram_id);
    usb_puts("FRAM ID = 0x");
    for (size_t i = 0 ; i < sizeof fram_id; i++)
        usb_printf("%02x", fram_id[i]);
    usb_puts("\n");
}

int fram_test(void) {
    // We are going to save in FRAM value equal to the its address %256 in fram
    //USE_MEMORY
    uint8_t fram_cache[256];
    int errors = 0;

    for(uint16_t i = 0; i < 1024; i++) {
        for(uint16_t u = 0; u <= 256; u ++) {
            // Test sleep and wake-up periodicaly
            if (u % 10 == 0)
                fram_sleep();
            fram_write8(u + i*256, u);
        }

        // Read as block
        fram_read(i*256, fram_cache, 256);

        // Check that the RAM returned the right content
        for (uint16_t u = 0; u < 256; u++) {
            if (fram_cache[u] != u) {
                usb_printf("Error: fram[%d] = %03d, expected %d\n", u + i*256,
                        fram_cache[u], u);
                errors++;
            }
        }
        if(errors) { // While no errors go on with the test, else make a pause
            usb_printf("FRAM Part test: %d\n", i + 1);
            usb_printf("FRAM read errors: %d\n", errors);
            usb_printf("FRAM status register: 0x%02x\n", fram_rdsr());
        }
    }
    if(errors)
        usb_puts("FRAM test: FAIL;\n");
    else
        usb_puts("FRAM test: SUCCESS;\n");

    return !errors;
}
