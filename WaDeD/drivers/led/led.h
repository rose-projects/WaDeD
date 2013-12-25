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
 * @file  led.h
 * @brief Some function for the LED.
 * */

#ifndef __LED_H__
#define __LED_H__

#ifndef HAL_H
#define HAL_H
#include "hal.h"

// LEDs control
#define LED_PWMD &PWMD2

#define LED_CHANNEL 2

// Initial value of the brigthness of the LCD (between 0 and 256)
#define LCD_BRIGHTNESS_INIT 255
#endif // HAL_H

/**
 * @brief Initialize PWM for LEDs.
 */
void led_init(void);

/**
 * Start a thread, doing a animation with the led.
 */
void led_thread(void);

/**
 * @brief Toggle the LED.
 */
void led_toggle(void);

#endif // __LED_H__
