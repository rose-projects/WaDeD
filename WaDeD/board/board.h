/*
 *  WaDeD - Short messages mesh network
 *
 *  ChibiOS/RT - Copyright (C) 2006,2007,2008,2009,2010,
 *               2011,2012,2013 Giovanni Di Sirio.
 *
 *  Copyright (C) 2013 ROSE <waded-rose@googlegroups.com>
 *
 *  This file is based on a file from ChibiOS/RT.
 *
 *  ChibiOS/RT is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  ChibiOS/RT is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _BOARD_H_
#define _BOARD_H_

// Setup for st stm32l-discovery board.
// Board identifier.
#define BOARD_ST_STM32L_DISCOVERY
#define BOARD_NAME                  "WaDeD"


// Board oscillators-related settings.
// Note: hse not fitted.
#if !defined(STM32_LSECLK)
#define STM32_LSECLK                32768
#endif

#if !defined(STM32_HSECLK)
#define STM32_HSECLK                4000000
#endif

// MCU type as defined in the st header file stm32l1xx.h.
#define STM32L1XX_MD

// IO pins assignments.
#define GPIOA_AUDIO_WKUP             0
#define GPIOA_AUDIO_VCC              1
#define GPIOA_SX_RXTX                2
#define GPIOA_SX_DIO5                3
#define GPIOA_SX_NSS                 4
#define GPIOA_SX_SCK                 5
#define GPIOA_SX_MISO                6
#define GPIOA_SX_MOSI                7
#define GPIOA_SX_DIO1                8
#define GPIOA_SX_DIO0                9
#define GPIOA_RF_RST                10
#define GPIOA_USB_DM                11
#define GPIOA_USB_DP                12
#define GPIOA_JTAG_TMS              13
#define GPIOA_JTAG_TCK              14
#define GPIOA_JTAG_TDI              15

#define GPIOB_SX_DIO4                0
#define GPIOB_SX_DIO3                1
#define GPIOB_BOOT1                  2
#define GPIOB_JTAG_TDO               3
#define GPIOB_JTAG_TRST              4
#define GPIOB_PIN5                   5
#define GPIOB_PIN6                   6
#define GPIOB_PIN7                   7
#define GPIOB_PIN8                   8
#define GPIOB_LIGHT_VCC              9
#define GPIOB_LED                   10
#define GPIOB_SX_DIO2               11
#define GPIOB_FRAM_NSS              12
#define GPIOB_FRAM_SCK              13
#define GPIOB_FRAM_MISO             14
#define GPIOB_FRAM_MOSI             15

#define GPIOC_LIGHT_WKUP            13
#define GPIOC_OSC32_IN              14
#define GPIOC_OSC32_OUT             15

#define GPIOH_OSC_IN                 0
#define GPIOH_OSC_OUT                1

/*
 * I/O ports initial setup, this configuration is established soon after reset
 * in the initialization code.
 * please refer to the stm32 reference manual for details.
 */
#define PIN_MODE_INPUT(N)           (0U << ((N) * 2))
#define PIN_MODE_OUTPUT(N)          (1U << ((N) * 2))
#define PIN_MODE_ALTERNATE(N)       (2U << ((N) * 2))
#define PIN_MODE_ANALOG(N)          (3U << ((N) * 2))
#define PIN_ODR_LOW(N)              (0U << (N))
#define PIN_ODR_HIGH(N)             (1U << (N))
#define PIN_OTYPE_PUSHPULL(N)       (0U << (N))
#define PIN_OTYPE_OPENDRAIN(N)      (1U << (N))
#define PIN_OSPEED_400K(N)          (0U << ((N) * 2))
#define PIN_OSPEED_2M(N)            (1U << ((N) * 2))
#define PIN_OSPEED_10M(N)           (2U << ((N) * 2))
#define PIN_OSPEED_40M(N)           (3U << ((N) * 2))
#define PIN_PUPDR_FLOATING(N)       (0U << ((N) * 2))
#define PIN_PUPDR_PULLUP(N)         (1U << ((N) * 2))
#define PIN_PUPDR_PULLDOWN(N)       (2U << ((N) * 2))
#define PIN_AFIO_AF(N, V)           ((V##U) << ((N % 8) * 4))

/*
 * GPIOA SETUP:
 * PIN   NAME        SET AS
 * PA0   AUDIO_WKUP  Alternate 0 (System)
 * PA1   AUDIO_VCC   Output Pushpull
 * PA2   SX_RXTX     Output Pushpull
 * PA3   SX_DIO5     Input Floating
 * PA4   SX_NSS      Output Pushpull (SPI1)
 * PA5   SX_SCK      Alternate  5    (SPI1)
 * PA6   SX_MISO     Alternate  5    (SPI1)
 * PA7   SX_MOSI     Alternate  5    (SPI1)
 * PA8   SX_DIO1     Input Floating
 * PA9   SX_DIO0     Input Floating
 * PA10  RF_RST      Output Pushpull
 * PA11  USB_D-      Alternate 10 (USB)
 * PA12  USB_D+      Alternate 10 (USB)
 * PA13  JTAG_TMS    Alternate  0 (System)
 * PA14  JTAG_TCK    Alternate  0 (System)
 * PA15  JTAG_TDI    Alternate  0 (System)
 */
#define VAL_GPIOA_MODER             (PIN_MODE_ALTERNATE(GPIOA_AUDIO_WKUP) | \
                                     PIN_MODE_OUTPUT(GPIOA_AUDIO_VCC)     | \
                                     PIN_MODE_OUTPUT(GPIOA_SX_RXTX)       | \
                                     PIN_MODE_INPUT(GPIOA_SX_DIO5)        | \
                                     PIN_MODE_OUTPUT(GPIOA_SX_NSS)        | \
                                     PIN_MODE_ALTERNATE(GPIOA_SX_SCK)     | \
                                     PIN_MODE_ALTERNATE(GPIOA_SX_MISO)    | \
                                     PIN_MODE_ALTERNATE(GPIOA_SX_MOSI)    | \
                                     PIN_MODE_INPUT(GPIOA_SX_DIO1)        | \
                                     PIN_MODE_INPUT(GPIOA_SX_DIO0)        | \
                                     PIN_MODE_OUTPUT(GPIOA_RF_RST)        | \
                                     PIN_MODE_INPUT(GPIOA_USB_DM)         | \
                                     PIN_MODE_INPUT(GPIOA_USB_DP)         | \
                                     PIN_MODE_ALTERNATE(GPIOA_JTAG_TMS)   | \
                                     PIN_MODE_ALTERNATE(GPIOA_JTAG_TCK)   | \
                                     PIN_MODE_ALTERNATE(GPIOA_JTAG_TDI))

#define VAL_GPIOA_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOA_AUDIO_WKUP) | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_AUDIO_VCC)  | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_RXTX)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_DIO5)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_NSS)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_SCK)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_MISO)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_MOSI)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_DIO1)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_SX_DIO0)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_RF_RST)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_DM)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_USB_DP)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTAG_TMS)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTAG_TCK)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOA_JTAG_TDI))

#define VAL_GPIOA_OSPEEDR           (PIN_OSPEED_400K(GPIOA_AUDIO_WKUP)    | \
                                     PIN_OSPEED_400K(GPIOA_AUDIO_VCC)     | \
                                     PIN_OSPEED_2M(GPIOA_SX_RXTX)         | \
                                     PIN_OSPEED_2M(GPIOA_SX_DIO5)         | \
                                     PIN_OSPEED_40M(GPIOA_SX_NSS)         | \
                                     PIN_OSPEED_2M(GPIOA_SX_SCK)          | \
                                     PIN_OSPEED_2M(GPIOA_SX_MISO)         | \
                                     PIN_OSPEED_2M(GPIOA_SX_MOSI)         | \
                                     PIN_OSPEED_2M(GPIOA_SX_DIO1)         | \
                                     PIN_OSPEED_2M(GPIOA_SX_DIO0)         | \
                                     PIN_OSPEED_400K(GPIOA_RF_RST)        | \
                                     PIN_OSPEED_40M(GPIOA_USB_DM)         | \
                                     PIN_OSPEED_40M(GPIOA_USB_DP)         | \
                                     PIN_OSPEED_40M(GPIOA_JTAG_TMS)       | \
                                     PIN_OSPEED_40M(GPIOA_JTAG_TCK)       | \
                                     PIN_OSPEED_40M(GPIOA_JTAG_TDI))

#define VAL_GPIOA_PUPDR             (PIN_PUPDR_FLOATING(GPIOA_AUDIO_WKUP) | \
                                     PIN_PUPDR_PULLUP(GPIOA_AUDIO_VCC)    | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_RXTX)      | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_DIO5)      | \
                                     PIN_PUPDR_FLOATING(GPIOA_SX_NSS)     | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_SCK)       | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_MISO)      | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_MOSI)      | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_DIO1)      | \
                                     PIN_PUPDR_PULLUP(GPIOA_SX_DIO0)      | \
                                     PIN_PUPDR_PULLUP(GPIOA_RF_RST)       | \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_DM)     | \
                                     PIN_PUPDR_FLOATING(GPIOA_USB_DP)     | \
                                     PIN_PUPDR_PULLUP(GPIOA_JTAG_TMS)     | \
                                     PIN_PUPDR_PULLUP(GPIOA_JTAG_TCK)     | \
                                     PIN_PUPDR_PULLUP(GPIOA_JTAG_TDI))

#define VAL_GPIOA_ODR               (PIN_ODR_HIGH(GPIOA_AUDIO_WKUP)       | \
                                     PIN_ODR_HIGH(GPIOA_AUDIO_VCC)        | \
                                     PIN_ODR_HIGH(GPIOA_SX_RXTX)          | \
                                     PIN_ODR_HIGH(GPIOA_SX_DIO5)          | \
                                     PIN_ODR_HIGH(GPIOA_SX_NSS)           | \
                                     PIN_ODR_HIGH(GPIOA_SX_SCK)           | \
                                     PIN_ODR_HIGH(GPIOA_SX_MISO)          | \
                                     PIN_ODR_HIGH(GPIOA_SX_MOSI)          | \
                                     PIN_ODR_HIGH(GPIOA_SX_DIO1)          | \
                                     PIN_ODR_HIGH(GPIOA_SX_DIO0)          | \
                                     PIN_ODR_LOW(GPIOA_RF_RST)            | \
                                     PIN_ODR_HIGH(GPIOA_USB_DM)           | \
                                     PIN_ODR_HIGH(GPIOA_USB_DP)           | \
                                     PIN_ODR_HIGH(GPIOA_JTAG_TMS)         | \
                                     PIN_ODR_HIGH(GPIOA_JTAG_TCK)         | \
                                     PIN_ODR_HIGH(GPIOA_JTAG_TDI))

#define VAL_GPIOA_AFRL              (PIN_AFIO_AF(GPIOA_AUDIO_WKUP, 0)     | \
                                     PIN_AFIO_AF(GPIOA_SX_SCK,     5)     | \
                                     PIN_AFIO_AF(GPIOA_SX_MISO,    5)     | \
                                     PIN_AFIO_AF(GPIOA_SX_MOSI,    5))

#define VAL_GPIOA_AFRH              (PIN_AFIO_AF(GPIOA_USB_DM,    10)     | \
                                     PIN_AFIO_AF(GPIOA_USB_DP,    10)     | \
                                     PIN_AFIO_AF(GPIOA_JTAG_TMS,   0)     | \
                                     PIN_AFIO_AF(GPIOA_JTAG_TCK,   0)     | \
                                     PIN_AFIO_AF(GPIOA_JTAG_TDI,   0))

/*
 * GPIOB setup:
 * PIN   NAME        SET AS
 * PB0   SX_DIO4     Input floating
 * PB1   SX_DIO3     Input floating
 * PB2   BOOT1       Input floating
 * PB3   JTAG_TDO    alternate 0 (JTAG)
 * PB4   JTAG_TRST   alternate 0 (JTAG)
 * PB5   PIN5        Input Pullup
 * PB6   PIN6        Input Pullup
 * PB7   PIN7        Input Pullup
 * PB8   PIN8        Input Pullup
 * PB9   LIGHT_VCC   Output pushpull
 * PB10  LED         Alternate 1 (TIM2)
 * PB11  SX_DIO2     Input floating
 * PB12  FRAM_NSS    Output PushPull (SPI2)
 * PB13  FRAM_SCK    Alternate 5     (SPI2)
 * PB14  FRAM_MISO   Alternate 5     (SPI2)
 * PB15  FRAM_MOSI   Alternate 5     (SPI2)
 */
#define VAL_GPIOB_MODER             (PIN_MODE_INPUT(GPIOB_SX_DIO4)        | \
                                     PIN_MODE_INPUT(GPIOB_SX_DIO3)        | \
                                     PIN_MODE_INPUT(GPIOB_BOOT1)          | \
                                     PIN_MODE_ALTERNATE(GPIOB_JTAG_TDO)   | \
                                     PIN_MODE_ALTERNATE(GPIOB_JTAG_TRST)  | \
                                     PIN_MODE_INPUT(GPIOB_PIN5)           | \
                                     PIN_MODE_INPUT(GPIOB_PIN6)           | \
                                     PIN_MODE_INPUT(GPIOB_PIN7)           | \
                                     PIN_MODE_INPUT(GPIOB_PIN8)           | \
                                     PIN_MODE_OUTPUT(GPIOB_LIGHT_VCC)     | \
                                     PIN_MODE_ALTERNATE(GPIOB_LED)        | \
                                     PIN_MODE_INPUT(GPIOB_SX_DIO2)        | \
                                     PIN_MODE_OUTPUT(GPIOB_FRAM_NSS)      | \
                                     PIN_MODE_ALTERNATE(GPIOB_FRAM_SCK)   | \
                                     PIN_MODE_ALTERNATE(GPIOB_FRAM_MISO)  | \
                                     PIN_MODE_ALTERNATE(GPIOB_FRAM_MOSI))

#define VAL_GPIOB_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOB_SX_DIO4)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SX_DIO3)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_BOOT1)      | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_JTAG_TDO)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_JTAG_TRST)  | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN5)       | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN6)       | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN7)       | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_PIN8)       | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_LIGHT_VCC)  | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_LED)        | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_SX_DIO2)    | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FRAM_NSS)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FRAM_SCK)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FRAM_MISO)  | \
                                     PIN_OTYPE_PUSHPULL(GPIOB_FRAM_MOSI))

#define VAL_GPIOB_OSPEEDR           (PIN_OSPEED_2M(GPIOB_SX_DIO4)         | \
                                     PIN_OSPEED_2M(GPIOB_SX_DIO3)         | \
                                     PIN_OSPEED_400K(GPIOB_BOOT1)         | \
                                     PIN_OSPEED_40M(GPIOB_JTAG_TDO)       | \
                                     PIN_OSPEED_40M(GPIOB_JTAG_TRST)      | \
                                     PIN_OSPEED_400K(GPIOB_PIN5)          | \
                                     PIN_OSPEED_400K(GPIOB_PIN6)          | \
                                     PIN_OSPEED_400K(GPIOB_PIN7)          | \
                                     PIN_OSPEED_400K(GPIOB_PIN8)          | \
                                     PIN_OSPEED_400K(GPIOB_LIGHT_VCC)     | \
                                     PIN_OSPEED_400K(GPIOB_LED)           | \
                                     PIN_OSPEED_400K(GPIOB_SX_DIO2)       | \
                                     PIN_OSPEED_40M(GPIOB_FRAM_NSS)       | \
                                     PIN_OSPEED_40M(GPIOB_FRAM_SCK)       | \
                                     PIN_OSPEED_40M(GPIOB_FRAM_MISO)      | \
                                     PIN_OSPEED_40M(GPIOB_FRAM_MOSI))

#define VAL_GPIOB_PUPDR             (PIN_PUPDR_FLOATING(GPIOB_SX_DIO4)    | \
                                     PIN_PUPDR_FLOATING(GPIOB_SX_DIO3)    | \
                                     PIN_PUPDR_FLOATING(GPIOB_BOOT1)      | \
                                     PIN_PUPDR_PULLUP(GPIOB_JTAG_TDO)     | \
                                     PIN_PUPDR_PULLUP(GPIOB_JTAG_TRST)    | \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN5)         | \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN6)         | \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN7)         | \
                                     PIN_PUPDR_PULLUP(GPIOB_PIN8)         | \
                                     PIN_PUPDR_PULLUP(GPIOB_LIGHT_VCC)    | \
                                     PIN_PUPDR_PULLUP(GPIOB_LED)          | \
                                     PIN_PUPDR_FLOATING(GPIOB_SX_DIO2)    | \
                                     PIN_PUPDR_FLOATING(GPIOB_FRAM_NSS)   | \
                                     PIN_PUPDR_PULLUP(GPIOB_FRAM_SCK)     | \
                                     PIN_PUPDR_PULLUP(GPIOB_FRAM_MISO)    | \
                                     PIN_PUPDR_PULLUP(GPIOB_FRAM_MOSI))

#define VAL_GPIOB_ODR               (PIN_ODR_HIGH(GPIOB_SX_DIO4)          | \
                                     PIN_ODR_HIGH(GPIOB_SX_DIO3)          | \
                                     PIN_ODR_HIGH(GPIOB_BOOT1)            | \
                                     PIN_ODR_HIGH(GPIOB_JTAG_TDO)         | \
                                     PIN_ODR_HIGH(GPIOB_JTAG_TRST)        | \
                                     PIN_ODR_HIGH(GPIOB_PIN5)             | \
                                     PIN_ODR_HIGH(GPIOB_PIN6)             | \
                                     PIN_ODR_HIGH(GPIOB_PIN7)             | \
                                     PIN_ODR_HIGH(GPIOB_PIN8)             | \
                                     PIN_ODR_HIGH(GPIOB_LIGHT_VCC)        | \
                                     PIN_ODR_HIGH(GPIOB_LED)              | \
                                     PIN_ODR_HIGH(GPIOB_SX_DIO2)          | \
                                     PIN_ODR_HIGH(GPIOB_FRAM_NSS)         | \
                                     PIN_ODR_HIGH(GPIOB_FRAM_SCK)         | \
                                     PIN_ODR_HIGH(GPIOB_FRAM_MISO)        | \
                                     PIN_ODR_HIGH(GPIOB_FRAM_MOSI))

#define VAL_GPIOB_AFRL              (PIN_AFIO_AF(GPIOB_BOOT1,     0)      | \
                                     PIN_AFIO_AF(GPIOB_JTAG_TDO,  0)      | \
                                     PIN_AFIO_AF(GPIOB_JTAG_TRST, 0)      | \
                                     PIN_AFIO_AF(GPIOB_PIN5,      0)      | \
                                     PIN_AFIO_AF(GPIOB_PIN6,      0)      | \
                                     PIN_AFIO_AF(GPIOB_PIN7,      0))


#define VAL_GPIOB_AFRH              (PIN_AFIO_AF(GPIOB_PIN8,      0)      | \
                                     PIN_AFIO_AF(GPIOB_LED,       1)      | \
                                     PIN_AFIO_AF(GPIOB_FRAM_SCK,  5)      | \
                                     PIN_AFIO_AF(GPIOB_FRAM_MISO, 5)      | \
                                     PIN_AFIO_AF(GPIOB_FRAM_MOSI, 5))

/*
 * GPIOC setup:
 * PIN   NAME        SET AS
 * PC13  LIGHT_WKUP  Alternate 0 (System)
 * PC14  OSC32_IN    Input Floating
 * PC15  OSC32_OUT   Input Floating
 */
#define VAL_GPIOC_MODER             (PIN_MODE_ALTERNATE(GPIOC_LIGHT_WKUP) | \
                                     PIN_MODE_INPUT(GPIOC_OSC32_IN)       | \
                                     PIN_MODE_INPUT(GPIOC_OSC32_OUT))

#define VAL_GPIOC_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOC_LIGHT_WKUP) | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_IN)   | \
                                     PIN_OTYPE_PUSHPULL(GPIOC_OSC32_OUT))

#define VAL_GPIOC_OSPEEDR           (PIN_OSPEED_400K(GPIOC_LIGHT_WKUP)    | \
                                     PIN_OSPEED_40M(GPIOC_OSC32_IN)       | \
                                     PIN_OSPEED_40M(GPIOC_OSC32_OUT))

#define VAL_GPIOC_PUPDR             (PIN_PUPDR_FLOATING(GPIOC_LIGHT_WKUP) | \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_IN)   | \
                                     PIN_PUPDR_FLOATING(GPIOC_OSC32_OUT))

#define VAL_GPIOC_ODR               (PIN_ODR_HIGH(GPIOC_LIGHT_WKUP)       | \
                                     PIN_ODR_HIGH(GPIOC_OSC32_IN)         | \
                                     PIN_ODR_HIGH(GPIOC_OSC32_OUT))

#define VAL_GPIOC_AFRL               0

#define VAL_GPIOC_AFRH              (PIN_AFIO_AF(GPIOC_LIGHT_WKUP,  0)    | \
                                     PIN_AFIO_AF(GPIOC_OSC32_IN,    0)    | \
                                     PIN_AFIO_AF(GPIOC_OSC32_OUT,   0))


// GPIOD setup : no D port
#define VAL_GPIOD_MODER              0

#define VAL_GPIOD_OTYPER             0

#define VAL_GPIOD_OSPEEDR            0

#define VAL_GPIOD_PUPDR              0

#define VAL_GPIOD_ODR                0

#define VAL_GPIOD_AFRL               0

#define VAL_GPIOD_AFRH               0

// GPIOE setup : no E port
#define VAL_GPIOE_MODER              0

#define VAL_GPIOE_OTYPER             0

#define VAL_GPIOE_OSPEEDR            0

#define VAL_GPIOE_PUPDR              0

#define VAL_GPIOE_ODR                0

#define VAL_GPIOE_AFRL               0

#define VAL_GPIOE_AFRH               0



/*
 * GPIOH setup:
 * PIN NAME              NAME       SET AS
 * PH0  - OSC_IN         OSC_IN     Input Floating
 * PH1  - OSC_OUT        OSC_OUT    Input Floating
 */
#define VAL_GPIOH_MODER             (PIN_MODE_INPUT(GPIOH_OSC_IN)         | \
                                     PIN_MODE_INPUT(GPIOH_OSC_OUT))

#define VAL_GPIOH_OTYPER            (PIN_OTYPE_PUSHPULL(GPIOH_OSC_IN)     | \
                                     PIN_OTYPE_PUSHPULL(GPIOH_OSC_OUT))

#define VAL_GPIOH_OSPEEDR           (PIN_OSPEED_40M(GPIOH_OSC_IN)         | \
                                     PIN_OSPEED_40M(GPIOH_OSC_OUT))

#define VAL_GPIOH_PUPDR             (PIN_PUPDR_FLOATING(GPIOH_OSC_IN)     | \
                                     PIN_PUPDR_FLOATING(GPIOH_OSC_OUT))

#define VAL_GPIOH_ODR               (PIN_ODR_HIGH(GPIOH_OSC_IN)           | \
                                     PIN_ODR_HIGH(GPIOH_OSC_OUT))

#define VAL_GPIOH_AFRL              (PIN_AFIO_AF(GPIOH_OSC_IN, 0)         | \
                                     PIN_AFIO_AF(GPIOH_OSC_OUT, 0))

#define VAL_GPIOH_AFRH              0


// USB bus activation macro, required by the USB driver.
#define usb_lld_connect_bus(usbp) SYSCFG->PMC |= SYSCFG_PMC_USB_PU

// USB bus de-activation macro, required by the USB driver.
#define usb_lld_disconnect_bus(usbp) SYSCFG->PMC &= ~SYSCFG_PMC_USB_PU

#if !defined(_FROM_ASM_)
#ifdef __cplusplus
extern "C" {
#endif
  void boardInit(void);
#ifdef __cplusplus
}
#endif
#endif /* _FROM_ASM_ */

#endif /* _BOARD_H_ */
