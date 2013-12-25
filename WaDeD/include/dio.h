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
 * @file  dio.h
 * @brief EXTI and functions for the DIO of the SX1231.
 * */

#ifndef __DIO_H__
#define __DIO_H__

#include "sx1231.h"

#define DIO0_MASK  (1 << 0)
#define DIO1_MASK  (1 << 1)
#define DIO2_MASK  (1 << 2)
#define DIO3_MASK  (1 << 3)
#define DIO4_MASK  (1 << 4)
#define DIO5_MASK  (1 << 5)

/*
 * DIO Mapping
 * DIO0: 00: RX: CRC_ok      TX: Packet_sent / 01: RX: payload_ready
 * DIO1: 00: All modes: Fifo_level
 * DIO2: 00: All modes: Fifo_not_empty
 * DIO3: 00: All modes: Fifo_full
 * DIO4: 10: RX: RX_ready                    / 01: RX: RSSI
 * DIO5: 11: All mode: Mode Ready
 */
/**
 * @brief
 */
#define check_crc_ok()         check_dio0()
#define check_packet_sent()    check_dio0()
#define check_payload_ready()  check_dio0()
#define check_fifo_level()     check_dio1()
#define check_fifo_not_empty() check_dio2()
#define check_fifo_full()      check_dio3()
#define check_rx_ready()       check_dio4()
#define check_rssi()           check_dio4()
#define check_mode_ready()     check_dio5()

#define wait_crc_ok()          wait_dio0(TRUE)
#define wait_packet_sent()     wait_dio0(TRUE)
#define wait_payload_ready()   wait_dio0(TRUE)
#define wait_fifo_level()      wait_dio1(TRUE)
#define wait_not_fifo_level()  wait_dio1(FALSE)
#define wait_fifo_not_empty()  wait_dio2(TRUE)
#define wait_fifo_empty()      wait_dio2(FALSE)
#define wait_fifo_full()       wait_dio3(TRUE)
#define wait_fifo_not_full()   wait_dio3(FALSE)
#define wait_rx_ready()        wait_dio4(TRUE)
#define wait_mode_ready()      wait_dio5(TRUE)

#define wait_cca_timeout(timeout) wait_dio4_timeout(timeout, FALSE)

/**
 * @brief Initialise the EXT, initialise the mapping of the DIO on the SX1231.
 */
void dio_init(void);

/**
 * @brief  Check the value of the specified dio.
 *
 * @return The value of the dio.
 */
#define CHECK_DIO(g,n)                                          \
    static inline bool_t check_dio##n (void)                    \
    {                                                           \
        return (!!palReadPad(GPIO##g , GPIO##g##_SX_DIO##n));   \
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
#define WAIT_DIO_PROTO(n)                                       \
    void wait_dio##n (bool_t value);                            \
    int wait_dio##n##_timeout(systime_t timeout, bool_t value);

CHECK_DIO(A,0)
CHECK_DIO(A,1)
CHECK_DIO(B,2)
CHECK_DIO(B,3)
CHECK_DIO(B,4)
CHECK_DIO(A,5)

WAIT_DIO_PROTO(0)
WAIT_DIO_PROTO(1)
WAIT_DIO_PROTO(2)
WAIT_DIO_PROTO(3)
WAIT_DIO_PROTO(4)
WAIT_DIO_PROTO(5)

/**
 * @brief Change the DIO mapping for having payload ready signal on DIO 1.
 *
 * @note  You need to change the DIO mapping of the dio0, in
 *  order to read payload ready.
 */
static inline void payload_ready_on_dio0(void)
{
    sx_write8(REG_DIO_MAPPING_1, (DIO0_MAPPING(0b01) |
                                  DIO1_MAPPING(0b00) |
                                  DIO2_MAPPING(0b00) |
                                  DIO3_MAPPING(0b00)));
}

/**
 * @brief Change the DIO mapping for having packet_sent signal on DIO 1.
 *
 * @note  This is the default mapping for DIO 1. You need to change the DIO
 * mapping of the dio0, in order to read packet_sent.
 */
static inline void packet_sent_on_dio0(void)
{
    sx_write8(REG_DIO_MAPPING_1, (DIO0_MAPPING(0b00) |
                                  DIO1_MAPPING(0b00) |
                                  DIO2_MAPPING(0b00) |
                                  DIO3_MAPPING(0b00)));
}

/**
 * @brief Change the DIO mapping for having rssi signal on DIO 4.
 *
 * @note  You need to change the DIO mapping of the dio0, in
 *  order to read rssi.
 */
static inline void rssi_on_dio4(void)
{
    sx_write8(REG_DIO_MAPPING_1,(DIO4_MAPPING(0b01) |
                                 DIO5_MAPPING(0b11) |
                                 CLK_OUT(CLK_OUT_OFF)));
}

/**
 * @brief Change the DIO mapping for having rx_ready signal on DIO 1.
 *
 * @note This is the default mapping for DIO 4. You need to change the DI0
 *  mapping of the dio0, in order to read rx_ready.
 */
static inline void rx_ready_on_dio4(void)
{
    sx_write8(REG_DIO_MAPPING_1,(DIO4_MAPPING(0b10) |
                                 DIO5_MAPPING(0b11) |
                                 CLK_OUT(CLK_OUT_OFF)));
}

/**
 * @brief Wait for a certain for a message to be received.
 *
 * @note  This function read payload_ready, the mapping must be set for
 * reading payload_ready if it has been changed previously.
 *
 * @param timeout     The time during which the function will wait for a
 * message.
 */
static inline bool_t wait_message(systime_t timeout)
{
    chEvtGetAndClearEvents(DIO0_MASK | DIO1_MASK);
    if (check_fifo_level() || check_payload_ready())
        return 1;
    chEvtWaitAnyTimeout(DIO0_MASK | DIO1_MASK, timeout);
    return check_fifo_level() || check_payload_ready();
}
#endif // __DIO_H__
