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
 * @file  fram.h
 * @brief Functions to communicate with the FRAM.
 * */

#ifndef __FRAM_H__
#define __FRAM_H__

#include <stdint.h>
#include <unistd.h>

// Basic low level communication functions.
// All other fram interaction functions or macros use only those two.

/**
 * @brief Read in burst on the fram.
 *
 * @param address    The address where to start to read.
 * @param buffer     The buffer to store the data.
 * @param size       The size of what we want to read.
 */
void fram_read(uint32_t address, void *buffer, size_t size);

/**
 * @brief  Read 8 bits, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The 8 bits read.
 */
uint8_t fram_read8(uint32_t address);

/**
 * @brief  Read 16 bits, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The 16 bits read.
 */
uint16_t fram_read16(uint32_t address);

/**
 * @brief  Read 32 bits, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The 32 bits read.
 */
uint32_t fram_read32(uint32_t address);

/**
 * @brief  Read 64 bits, in the ram.
 *
 * @param  address    The address where to start to read.
 *
 * @return The 64 bits read.
 */
uint64_t fram_read64(uint32_t address);

/**
 * @brief Write in burst on the fram.
 *
 * @param address    The address to write.
 * @param buffer     The buffer containing the data.
 * @param size       Data length.
 */
void fram_write(uint32_t address, const void *buffer, size_t size);

/**
 * @brief Write 8 bits on the fram.
 *
 * @param address    The address to write.
 * @param data       The data to write.
 */
void fram_write8(uint32_t address, uint8_t data);

/**
 * @brief Write 16 bits on the fram.
 *
 * @param address    The address to write.
 * @param data       The data to write.
 */
void fram_write16(uint32_t address, uint16_t data);

/**
 * @brief Write 32 bits on the fram.
 *
 * @param address    The address to write.
 * @param data       The data to write.
 */
void fram_write32(uint32_t address, uint32_t data);

/**
 * @brief Write 64 bits on the fram.
 *
 * @param address    The address to write.
 * @param data       The data to write.
 */
void fram_write64(uint32_t address, uint64_t data);


/**
 * @brief  Read the status register of the FRAM.
 *
 * @return The status register of the FRAM.
 */
uint8_t fram_rdsr(void);

/**
 * @brief  Read the ID of the FRAM.
 *
 * @return The ID of the FRAM.
 */
void fram_rdid(uint8_t content[9]);

/**
 * @brief Put the FRAM in sleep mode.
 */
void fram_sleep(void);

/**
 * @brief Initialise the SPI for the FRAM.
 */
void fram_init(void);

#ifdef __SIMU__
extern char *fram;
#endif

#endif // __FRAM_H__
