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
 * @file  led.c
 * @brief Some function for the LED.
 * */

#include "led.h"

/**
 * Configuration of PWM from Timer 2 for the LED.
 */
static const PWMConfig pwmcfg2 = {
    20000,       // 78.125Hz PWM clock frequency.
    256,         // Period.
    NULL,        // No callback.
    {
        {PWM_OUTPUT_DISABLED, NULL},    // Nothing.
        {PWM_OUTPUT_DISABLED, NULL},    // Nothing.
        {PWM_OUTPUT_ACTIVE_HIGH, NULL}, // Led.
        {PWM_OUTPUT_DISABLED, NULL},    // Nothing.
    },
    0
};

static WORKING_AREA(waLED_Thread1, 256); /**< The LED thread working area. */

/**
 * @brief LED thread.
 *
 * @param arg Unused.
 *
 * @return Do not return.
 */
__attribute__((noreturn)) static msg_t
LED_Thread1(void *arg __attribute__((unused))) {
    chRegSetThreadName("LED Thread");

    while(TRUE) {
        for(int i = 0; i < 33; i++) {
            pwmEnableChannel(LED_PWMD, LED_CHANNEL, i * 8);
            chThdSleepMilliseconds(50);
        }
        pwmEnableChannel(LED_PWMD, LED_CHANNEL, 0);
    }
}

void led_init(void) {
    pwmStart(LED_PWMD, &pwmcfg2);
}

void led_thread(void) {
    chThdCreateStatic(waLED_Thread1, sizeof waLED_Thread1 , NORMALPRIO,
            LED_Thread1, NULL);
}

static char toggle = 0; /**< The last state of the LED, used to toggle it. */

void led_toggle(void)
{
    if(toggle)
        pwmEnableChannel(LED_PWMD, LED_CHANNEL, 0);
    else
        pwmEnableChannel(LED_PWMD, LED_CHANNEL, 255);
    toggle = !toggle;
}
