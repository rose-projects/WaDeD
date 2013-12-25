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
 * @file sx1231.c
 * @brief Low level functions for controlling the sx1231.
 * */

#include "sx1231.h"
#include "dio.h"
#include "global.h"

#define SPI_SX              &SPID1
#define SPI_CONFIG_SX       &spi_cfg_sx

// SPI configuration for the SX1231 (4MHz, CPHA=0, CPOL=0, MSb first)
static const SPIConfig spi_cfg_sx = {
    NULL,
    GPIOA,
    4,
    0x10
};

/**
 * @brief Reset the SX1231 and initialised the SPI for the SX1231,
 */
void sx_init(void)
{
    // SX1231 reset
    palSetPad(GPIOA, GPIOA_RF_RST);
    chThdSleepMilliseconds(100);
    palClearPad(GPIOA, GPIOA_RF_RST);
    chThdSleepMilliseconds(100);

    // Start SPI
    spiStart(SPI_SX, SPI_CONFIG_SX);
}

/**
 * @brief Start a SPI transaction.
 */
static int start_spi(void)
{
    spiAcquireBus(SPI_SX);
    spiSelect(SPI_SX);
    return 0;
}

/**
 * @brief End a SPI transaction.
 */
static int end_spi(void)
{
    spiUnselect(SPI_SX);
    spiReleaseBus(SPI_SX);
    return 0;
}

#define spi_send(data, size) spiSend(SPI_SX, (size), (data))
#define spi_receive(buffer, size) do {                                  \
        assert((size) < 256);                                           \
        spiReceive(SPI_SX, (size), (buffer));                           \
    } while(0)

/**
 * @brief Send a byte from SPI.
 *
 * @param b    The byte to send.
 */
static inline void spi_send8(uint8_t b)
{
    spi_send(&b, sizeof b);
}

/**
 * @def Pre-processor function, to do a SPI transaction.
 */
#define SPITRANSACTION for(int __first = start_spi() || 1; __first; __first = end_spi())

/**
 * @brief Read in burst the register of the SX1231.
 *
 * @note The burst reading works on every register except REG_FIFO. If you do
 * a burst from REG_FIFO you will read n times the FIFO.
 *
 * @param address   First address to be read.
 * @param data      Pointer of the buffer where the read value will be stocked.
 * @param size      Number of register to read
 */
void sx_read(uint8_t address, void *data, size_t size)
{
    SPITRANSACTION {
        spi_send8(address & 0x7F);
        spi_receive(data, size);
    }
}

/**
 * @brief Make reading access with ignoring what is returned by the SX1231.
 *
 * @param address   First address to be read.
 * @param data      Pointer of the buffer where the read value will be stocked.
 * @param size      Number of register to read
 */
void sx_read_dummy(uint8_t address, size_t size)
{
    SPITRANSACTION {
        spi_send8(address & 0x7F);
        spiIgnore(SPI_SX, size);
    }
}


/**
 * @brief Write in burst in the register of the SX1231.
 *
 * @note The burst writing works on every register except REG_FIFO. If you do
 * a burst from REG_FIFO you will write n times in the FIFO.
 *
 * @param address   First address in which we will write.
 * @param data      Pointer of the buffer containing the data to write.
 * @param size      Number of register to write
 */
void sx_write(uint8_t address, const void *data, size_t size)
{
    SPITRANSACTION {
        spi_send8(address | (1<<7));
        spi_send(data, size);
    }
}

/**
 * @brief Write a byte in a register of the SX1231.
 *
 * @param address    The address of the register to write in.
 * @param data       The byte to write.
 */
void sx_write8(uint8_t address, uint8_t data)
{
    sx_write(address, &data, 1);
}

/**
 * @brief Write two bytes in two consecutive registers of the SX1231.
 *
 * @param address    The address of the first register where we will write.
 * @param data       The bytes to write.
 */
void sx_write16(uint8_t address, uint16_t data)
{
    uint16_t data_be = ((data & 0xff) << 8) | ((data & 0xff00) >> 8);
    sx_write(address, &data_be, 2);
}

/**
 * @brief Write three bytes in three conecutive registers of the SX1231.
 *
 * @param address    The address of the register to read.
 * @param data       The bytes to write.
 */
void sx_write24(uint8_t address, uint32_t data)
{
    uint32_t data_be = (((data & 0xff) << 16) | ((data & 0xff00)) |
                       ((data & 0xff0000) >> 16));
    sx_write(address, &data_be, 3);
}

/**
 * @brief Read a byte/register of the SX1231.
 *
 * @param address    The address of the register to read.
 * @return The read byte.
 */
uint8_t sx_read8(uint8_t address)
{
    uint8_t data;
    sx_read(address, &data, 1);
    return data;
}

/************************* SX1231 Settings functions ***********************/

/**
 * @brief Put the SX1231 in the selected mode.
 *
 * @param mode SX1231_MODE
 */
void sx_mode(uint8_t mode)
{
    sx_write8(REG_OP_MODE, ((sx_read8(REG_OP_MODE) & !(MODE(0b11))) |
                            MODE(mode)));
    wait_mode_ready();
    if (mode == RX)
        chThdSleepMilliseconds(100); // wait_rx_ready();
}


/**
 * @brief Set the value of the RSSI threshold.
 *
 * @param rssi_thresh  Value of the rssi threshold to be setted.
 * Between 0 and 66.
 */
uint8_t scan_rssi(void)
{
    sx_write8(REG_RSSI_CONFIG, RSSI_START(ON));
    while(!(sx_read8(REG_RSSI_CONFIG) & RSSI_DONE)) ;
    return sx_read8(REG_RSSI_VALUE);
}

/**
 * @brief Set the value of the RSSI threshold.
 *
 * @param rssi_thresh  Value of the rssi threshold to be setted.
 * Between 0 and 66.
 */
void set_rssi_thresh(uint8_t rssi_thresh)
{
    sx_write8(REG_RSSI_THRESH, rssi_thresh);
}

/**
 * @brief Set the gain of the SX1231 in transmission.
 *
 * @param gain  SX1231_GAIN
 */
void set_gain(uint8_t gain)
{
    sx_write8(REG_LNA, LNA_ZIN(Z_50) | LNA_GAIN_SELECT(gain));
}

/**
 * @brief Empty the FIFO of the SX1231.
 */
void empty_fifo(void)
{
    while (check_fifo_not_empty())
        sx_read_dummy(REG_FIFO, check_fifo_level() ? VALUE_FIFO_THRESH : 1);
}

/************************ Communication functions ****************************/
/**
 * @param a   First size to evaluate.
 * @param b   Second size to evaluate.
 *
 * @return The min between a and b.
 */
static inline size_t min(size_t a, size_t b)
{
    return a < b ? a : b;
}

/**
 * @brief Send a packet through radio.
 *
 * @param packet    Pointer to the buffer containing the data to send.
 * @param size      Size of the data to sned. Cannot be up to 255.
 * @param timeout   Time during which we try to send a message.
 *
 * @return SX1231_ERR
 */
int send_packet(const void *packet, size_t size, systime_t timeout)
{
    // Wait for RSSI level
    sx_mode(RX);
    rssi_on_dio4();
    int channel_clear = wait_cca_timeout(timeout);
    rx_ready_on_dio4();
    if (!channel_clear) {
        sx_mode(STDBY);
        return CHANNEL_BUSY;
    }

    sx_mode(STDBY);

    packet_sent_on_dio0();
    sx_write8(REG_FIFO, size);
    {
        size_t sent = min(size, FIFO_SIZE - 1);
        sx_write(REG_FIFO, packet, sent);
        packet += sent;
        size -= sent;
    }
    sx_mode(TX);
    while (size) {
        wait_not_fifo_level();
        size_t sent = min(size, FIFO_SIZE - VALUE_FIFO_THRESH - 1);
        sx_write(REG_FIFO, packet, sent);
        packet += sent;
        size -= sent;
    }
    wait_packet_sent();
    assert(!check_fifo_not_empty());
    chThdSleepMilliseconds(10);
    sx_mode(STDBY);
    return TX_SUCCESS;
}

/**
 * @brief Pass the SX1231 in RX mode and try to catch a radio packet.
 *
 * @param  packet            Buffer where the packet received will be stocked.
 * @param  max_size          Maximum size allowed for receiving a packet.
 * @param  timeout           Time during which we wait for a packet.
 *
 * @return The size of the packet, if the packet is correct or SX1231_ERR
 */
int receive_packet(void *packet, size_t max_size, systime_t timeout)
{
    payload_ready_on_dio0(); // In order to check payload ready

    sx_write8(REG_PAYLOAD_LENGTH, max_size);

    sx_mode(RX);

    if (!wait_message(timeout))
        return TIMEOUT;

    size_t size          = sx_read8(REG_FIFO);
    ssize_t bytes_left   = size;

    while (bytes_left > 0) {
        while (wait_message(TIME_INFINITE) == 0) ;
        if (check_payload_ready()) {
            // CRC_OK flag is reset when going out of RX mode
            if (!(sx_read8(REG_IRQ_FLAGS_2) & FLAG_2_CRC_OK)) {
                sx_mode(STDBY);
                empty_fifo();
                return RECEIVE_CRC_ERROR;
            }
            sx_mode(STDBY);
            sx_read(REG_FIFO, packet, bytes_left);
            assert(!check_fifo_not_empty());
            return size;
        } else {
            if (!(bytes_left >= VALUE_FIFO_THRESH))
                return RECEIVE_TOO_LONG;
            sx_read(REG_FIFO, packet, VALUE_FIFO_THRESH);
            bytes_left -= VALUE_FIFO_THRESH;
            packet     += VALUE_FIFO_THRESH;
        }
    }
    return 0;  // Empty packet, should not happen
}
