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
 * @file  dio.c
 * @brief EXTI and functions for the DIO of the SX1231.
 * */

#include "ch.h"
#include "hal.h"
#include "dio.h"
#include "dash7.h"

static EVENTSOURCE_DECL(dio0_event);
static EVENTSOURCE_DECL(dio1_event);
static EVENTSOURCE_DECL(dio2_event);
static EVENTSOURCE_DECL(dio3_event);
static EVENTSOURCE_DECL(dio4_event);
static EVENTSOURCE_DECL(dio5_event);

/**
 * @brief Callback functions for the dio n
 *        Broadcast the event mask associated to the dio.
 */
#define DIOCB(n)                                                    \
    static void dio##n##cb(EXTDriver *extp, expchannel_t channel)   \
    {                                                               \
        (void) extp;                                                \
        (void) channel;                                             \
        chSysLockFromIsr();                                         \
        chEvtBroadcastFlagsI(&dio##n##_event, DIO##n##_MASK);       \
        chSysUnlockFromIsr();                                       \
    }

DIOCB(0)
DIOCB(1)
DIOCB(2)
DIOCB(3)
DIOCB(4)
DIOCB(5)

static const EXTConfig extcfg = {
    {
        // 0: GPIOB DIO4
        {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, dio4cb},
        // 1: GPIOB DIO3
        {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, dio3cb},
        // 2: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 3: GPIOA DIO5
        {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, dio5cb},
        // 4: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 5: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 6: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 7: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 8: GPIOA DIO1
        {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, dio1cb},
        // 9: GPIOA DIO0
        {EXT_CH_MODE_RISING_EDGE | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOA, dio0cb},
        // 10: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 11: GPIOB DIO2
        {EXT_CH_MODE_BOTH_EDGES | EXT_CH_MODE_AUTOSTART | EXT_MODE_GPIOB, dio2cb},
        // 12: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 13: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 14: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
        // 15: Nothing
        {EXT_CH_MODE_DISABLED, NULL},
    }
};

#define DIO_LISTENER_INIT(n)                                            \
    static EventListener dio##n##_listener;                             \
    chEvtRegisterMask(&dio##n##_event, &dio##n##_listener, DIO##n##_MASK);

/**
 * @brief Initialise the EXT, initialise the mapping of the DIO on the SX1231.
 */
void dio_init(void)
{
    // DIO Mapping initialisation
    sx_write8(REG_DIO_MAPPING_1,
              DIO0_MAPPING(0b00) |  // RX: CRC_OK           TX: Packet Sent
              DIO1_MAPPING(0b00) |  // All modes: Fifo level
              DIO2_MAPPING(0b00) |  // All modes: Fifo not empty
              DIO3_MAPPING(0b00));  // All modes: Fifo Full

    sx_write8(REG_DIO_MAPPING_2,
              DIO4_MAPPING(0b10) |  // RX: RX ready
              DIO5_MAPPING(0b11) |  // All modes: Mode ready
              CLK_OUT(CLK_OUT_OFF));


    extStart(&EXTD1, &extcfg);


    DIO_LISTENER_INIT(0)
        DIO_LISTENER_INIT(1)
        DIO_LISTENER_INIT(2)
        DIO_LISTENER_INIT(3)
        DIO_LISTENER_INIT(4)
        DIO_LISTENER_INIT(5)
        }


/**
 * @fn    Wait_dion(bool_t value)
 * @brief Wait for the DIO n to be at a state.
 *
 * @param value The state we want the DIO to wait.
 */
/**
 * @fn     wait_dion_timeout(systime_t timeout, bool_t value);
 * @brief  Wait for the DIO n to be at a state, for a certain time.
 *
 * @param  timeout    The during which we will wait for the DIO to be at the
 *                    state.
 * @param  value      The state we want the DIO to wait.
 *
 * @return Returns 1 if the value is the one we want.
 * @return Returns 0 if we reach the timeout.
 */
#define WAIT_DIO(n)                                             \
    void wait_dio##n (bool_t value)                             \
    {                                                           \
        while (check_dio##n () != value)                        \
            chEvtWaitOne(DIO##n##_MASK);                        \
    }                                                           \
                                                                \
    int wait_dio##n##_timeout(systime_t timeout, bool_t value)  \
    {                                                           \
        if (check_dio##n () == value)                           \
            return 1;                                           \
        systime_t deadline = chTimeNow() + timeout;             \
        int32_t delay;                                          \
        while ((delay = deadline - chTimeNow()) > 0) {          \
            chEvtWaitOneTimeout(DIO##n##_MASK, delay);          \
            if (check_dio##n () == value)                       \
                return 1;                                       \
        }                                                       \
        return 0;                                               \
    }

WAIT_DIO(0)
WAIT_DIO(1)
WAIT_DIO(2)
WAIT_DIO(3)
WAIT_DIO(4)
WAIT_DIO(5)
