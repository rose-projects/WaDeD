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
 * @file  fram.c
 * @brief Functions to communicate with the FRAM.
 */

#include "fram.h"
#include "hal.h"

#define SPI_FRAM        &SPID2
#define SPI_CONFIG_FRAM &spi_cfg_fram

// Op-code Commands
#define WRSR            0x01
#define WRITE           0x02
#define READ            0x03
#define WRDI            0x04
#define RDSR            0x05
#define WREN            0x06
#define FSTRD           0x0B
#define RDID            0x9F
#define SLEEP           0xB9

// Set when we have put the FRAM in sleep mode
static volatile int sleep_mode = 1;

// Maximum speed SPI configuration (16MHz, CPHA=0, CPOL=0, MSb first).
static const SPIConfig spi_cfg_fram = {
    NULL,
    GPIOB,
    12,
    0
};

/**
 * @brief  Start a SPI transaction.
 *
 * @return Returns 0.
 */
static int start_spi(void)
{
    spiAcquireBus(SPI_FRAM);
    spiSelect(SPI_FRAM);
    if (sleep_mode) {
        chThdSleepMicroseconds(400);
        sleep_mode = 0;
    }
    return 0;
}

/**
 * @brief  End a SPI transaction
 *
 * @return Returns 0.
 */
static int end_spi(void)
{
    spiUnselect(SPI_FRAM);
    spiReleaseBus(SPI_FRAM);
    return 0;
}

#define spi_send(data, size) spiSend(SPI_FRAM, (size), (data))
#define spi_receive(buffer, size) spiReceive(SPI_FRAM, (size), (buffer))

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
 * @brief Receive a byte from SPI.
 *
 * @return Returns the byte received.
 */
static inline uint8_t spi_receive8(void)
{
    uint8_t b;
    spi_receive(&b, sizeof b);
    return b;
}

/**
 * @brief Send the address we want to reach to the FRAM.
 *
 * @param addr    The address.
 */
static inline void spi_send_address(uint32_t addr)
{
    spi_send8((addr >> 16) & 3);
    spi_send8(addr >> 8);
    spi_send8(addr);
}

/**
 * @brief Pre-processor function, to do a SPI transaction.
 */
#define SPITRANSACTION for(int __first = start_spi() || 1; __first; \
                           __first = end_spi())

/**
 * @brief Initialise the SPI for the FRAM.
 */
void fram_init(void)
{
    spiStart(SPI_FRAM, SPI_CONFIG_FRAM);
}

/**
 * @brief Read in burst on the fram.
 *
 * @param address    The address where to start to read.
 * @param buffer     The buffer to store the data.
 * @param size       The size of what we want to read.
 */
void fram_read(uint32_t address, void *buffer, size_t size)
{
    SPITRANSACTION {
        // Send command
        spi_send8(READ);
        spi_send_address(address);
        // Receive Data
        spi_receive(buffer, size);
    }
}

/**
 * @brief Write in burst on the fram.
 *
 * @param address    The address to write.
 * @param buffer     The buffer containing the data.
 * @param size       Data length.
 */
void fram_write(uint32_t address, const void *buffer, size_t size)
{
    SPITRANSACTION
        spi_send8(WREN);
    SPITRANSACTION {
        // Send command
        spi_send8(WRITE);
        spi_send_address(address);
        // Send data
        spi_send(buffer, size);
    }
}

/**
 * @brief Put the FRAM in sleep mode.
 */
void fram_sleep(void)
{
    if (!sleep_mode) {
        SPITRANSACTION {
            spi_send8(SLEEP);
            sleep_mode = 1;
        }
    }
}

/**
 * @brief  Read the status register of the FRAM.
 *
 * @return The status register of the FRAM.
 */
uint8_t fram_rdsr(void)
{
    uint8_t b;
    SPITRANSACTION {
        spi_send8(RDSR);
        b = spi_receive8();
    }
    return b;
}

/**
 * @brief  Read the ID of the FRAM.
 *
 * @return The ID of the FRAM.
 */
void fram_rdid(uint8_t content[9])
{
    SPITRANSACTION {
        spi_send8(RDID);
        spi_receive(content, 9);
    }
}

#define WRITEOP(n)                                           \
    void fram_write##n(uint32_t address, uint##n##_t data)  \
    {                                                       \
        fram_write(address, &data, sizeof data);            \
    }

/**
 * @fn fram_write8(uint32_t address, uint8_t data)
 *
 * @brief  Read a byte, in the ram.
 *
 * @param  address    The address where to start to write.
 * @param  data       The data to write.
 */
WRITEOP(8)

/**
 * @fn fram_write16(uint32_t address, uint16_t data)
 *
 * @brief  Read 2 bytes, in the ram.
 *
 * @param  address    The address where to start to write.
 * @param  data       The data to write.
 */
WRITEOP(16)

/**
 * @fn fram_write32(uint32_t address)
 *
 * @brief  Read 4 bytes, in the ram.
 *
 * @param  address    The address where to start to write.
 * @param  data       The data to write.
 */
WRITEOP(32)

/**
 * @fn fram_write64(uint32_t address)
 *
 * @brief  Read 8 bytes, in the ram.
 *
 * @param  address    The address where to start to write.
 * @param  data       The data to write.
 */
WRITEOP(64)

#define READOP(n)                                           \
    uint##n##_t fram_read##n(uint32_t address)              \
    {                                                       \
        uint##n##_t data;                                   \
        fram_read(address, &data, sizeof data);              \
        return data;                                        \
    }
/**
 * @fn fram_read8(uint32_t address)
 *
 * @brief  Read a byte, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The uint8_t read.
 */
READOP(8)

/**
 * @fn fram_read16(uint32_t address)
 *
 * @brief  Read 2 bytes, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The uint16_t read.
 */
READOP(16)

/**
 * @fn fram_read32(uint32_t address)
 *
 * @brief  Read 4 bytes, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The uint32_t read.
 */
READOP(32)

/**
 * @fn fram_read64(uint32_t address)
 *
 * @brief  Read 8 bytes, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The uint64_t read
 */
READOP(64)
