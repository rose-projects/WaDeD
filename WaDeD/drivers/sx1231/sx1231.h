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
 * @file  sx1231.h
 * @brief Functions to handle the SX1231.
 */

#ifndef __SX1231_H___
#define __SX1231_H___

#include <stdint.h>
#include <unistd.h>
#include "hal.h"

//==========================================================================//
// Hardware constant                                                        //
//==========================================================================//

#define FIFO_SIZE 66

//==========================================================================//
// Registers address                                                        //
//==========================================================================//

#define REG_FIFO                        0x00
#define REG_OP_MODE                     0x01
#define REG_DATA_MODUL                  0x02
#define REG_BITRATE_MSB                 0x03
#define REG_BITRATE_LSB                 0x04
#define REG_FDEV_MSB                    0x05
#define REG_FDEV_LSB                    0x06
#define REG_FRF_MSB                     0x07
#define REG_FRF_MID                     0x08
#define REG_FRF_LSB                     0x09
#define REG_OSC_1                       0x0A
#define REG_AFC_CTRL                    0x0B
#define REG_LOW_BAT                     0x0C
#define REG_LISTEN_1                    0x0D
#define REG_LISTEN_2                    0x0E
#define REG_LISTEN_3                    0x0F
#define REG_VERSION                     0x10
#define REG_PA_LEVEL                    0x11
#define REG_PA_RAMP                     0x12
#define REG_OCP                         0x13
#define REG_LNA                         0x18
#define REG_RX_BW                       0x19
#define REG_AFC_BW                      0x1A
#define REG_OOK_PEAK                    0x1B
#define REG_OOK_AVG                     0x1C
#define REG_OOK_FIX                     0x1D
#define REG_AFC_FEI                     0x1E
#define REG_AFC_MSB                     0x1F
#define REG_AFC_LSB                     0x20
#define REG_FEI_MSB                     0x21
#define REG_FEI_LSB                     0x22
#define REG_RSSI_CONFIG                 0x23
#define REG_RSSI_VALUE                  0x24
#define REG_DIO_MAPPING_1               0x25
#define REG_DIO_MAPPING_2               0x26
#define REG_IRQ_FLAGS_1                 0x27
#define REG_IRQ_FLAGS_2                 0x28
#define REG_RSSI_THRESH                 0x29
#define REG_RX_TIMEOUT_1                0x2A
#define REG_RX_TIMEOUT_2                0x2B
#define REG_PREAMBLE_MSB                0x2C
#define REG_PREAMBLE_LSB                0x2D
#define REG_SYNC_CONFIG                 0x2E
#define REG_SYNC_VALUE_1                0x2F
#define REG_SYNC_VALUE_2                0x30
#define REG_SYNC_VALUE_3                0x31
#define REG_SYNC_VALUE_4                0x32
#define REG_SYNC_VALUE_5                0x33
#define REG_SYNC_VALUE_6                0X34
#define REG_SYNC_VALUE_7                0x35
#define REG_SYNC_VALUE_8                0x36
#define REG_PACKET_CONFIG_1             0x37
#define REG_PAYLOAD_LENGTH              0x38
#define REG_NODE_ADRS                   0x39
#define REG_BROADCAST_ADRS              0x3A
#define REG_AUTO_MODES                  0x3B
#define REG_FIFO_THRESH                 0x3C
#define REG_PACKET_CONFIG_2             0x3D
#define REG_AES_KEY_1                   0x3E
#define REG_AES_KEY_2                   0x3F
#define REG_AES_KEY_3                   0x40
#define REG_AES_KEY_4                   0x41
#define REG_AES_KEY_5                   0x42
#define REG_AES_KEY_6                   0x43
#define REG_AES_KEY_7                   0x44
#define REG_AES_KEY_8                   0x45
#define REG_AES_KEY_9                   0x46
#define REG_AES_KEY_10                  0x47
#define REG_AES_KEY_11                  0x48
#define REG_AES_KEY_12                  0x49
#define REG_AES_KEY_13                  0x4A
#define REG_AES_KEY_14                  0x4B
#define REG_AES_KEY_15                  0x4C
#define REG_AES_KEY_16                  0x4D
#define REG_TEMP_1                      0x4E
#define REG_TEMP_2                      0x4F
#define REG_TEST_1                      0x50  /* 8 addresses are available
                                               * for tests from this one */
#define REG_TEST_LNA                    0x58
#define REG_TEST_2                      0x59  /* 21 addresses are available
                                               * for tests from this one */
#define REG_TEST_DAGC                   0x6
#define REG_TEST_3                      0x70
#define REG_TEST_AFC                    0x71
#define REG_TEST_4                      0x72  /* 14 addresses are available
                                               * for tests from this one */

//==========================================================================//
// Registers address                                                        //
//==========================================================================//

// REG_OP_MODE functions
#define SEQUENCER_ON(N)                 ((N) << 7)
#define LISTEN_ON(N)                    ((N) << 6)
#define LISTEN_ABORT_ON(N)              ((N) << 5)
#define MODE(N)                         ((N) << 2)

// SEQUENCER_ON/LISTEN_ON/LISTEN_ABORT_ON arguments
#define OFF                             0b0    // Default (for all of them)
#define ON                              0b1

/**
 * @def SX1231_MODE
 * @brief REG_OP_MODE arguments
 * @{
 */
#define SLEEP                           0b000
#define STDBY                           0b001  // Default
#define FS                              0b010
#define TX                              0b011
#define RX                              0b100
/** @} */



// RG_DATA_MODUL functions
#define DATA_MODE(N)                    ((N) << 5)
#define MODULATION_TYPE(N)              ((N) << 3)
#define MODULATION_SHAPING(N)           ((N) << 0)

// DATA_MODE arguments
#define PACKET_MODE                     0b00   // Default
#define CONTINUOUS_MODE_SYNC            0b10
#define CONTINUOUS_MODE_NOSYNC          0b10

// MODULATION_TYPE arguments
#define FSK                             0b00   // Default
#define OOK                             0b01

// MODULATION_SHAPING arguments
// /!\ See datasheet p.63 for the type of the filters
#define NO_SHAPING                      0b00   // Default
#define FILTER_1                        0b01
#define FILTER_2                        0b10
#define FILTER_3                        0b11




// REG_AFC_CTRL function
#define AFC_LOW_BETA_ON(N)              ((N) << 5)

// AFC_LOW_BETA_ON arguments (already defined)
// OFF                                  0b0    // Default
// ON                                   0b1



// REG_LOW_BAT functions
#define LOW_BAT_ON(N)                   ((N) << 3)
#define LOW_BAT_TRIM(N)                 ((N) << 0)

// LOW_BAT_ON arguments (already defined)
// OFF                                  0b0    // Default
// ON                                   0b1

// LOW_BAT_TRIM arguments
#define LEVEL_0                         0b000  // 1.695V
#define LEVEL_1                         0b001  // 1.764V
#define LEVEL_2                         0b010  // 1.835V    Default
#define LEVEL_3                         0b011  // 1.905V
#define LEVEL_4                         0b100  // 1.976V
#define LEVEL_5                         0b101  // 2.045V
#define LEVEL_6                         0b110  // 2.116V
#define LEVEL_7                         0b111  // 2.185V




// REG_LISTEN_1 functions
#define LISTEN_RESOL_IDLE(N)            ((N) << 6)
#define LISTEN_RESOL_RX(N)              ((N) << 4)
#define LISTEN_CRITERIA(N)              ((N) << 3)
#define LISTEN_END(N)                   ((N) << 1)

// LISTEN_RESOL_IDLE/LISTEN_RESOL_RX arguments
#define LISTEN_64US                     0b01   // Default
#define LISTEN_4100US                   0b10
#define LISTEN_262MS                    0b11

// LISTEN_CRITERIA arguments
#define RSSI_ONLY                       0b0    // Default
#define RSSI_AND_ADDRESS                0b1

// LISTEN_END arguments
// /!\ See datasheet p.65 for the definition of the type
#define END_TYPE_1                      0b00
#define END_TYPE_2                      0b01   // Default
#define END_TYPE_3                      0b10
#define END_TYPE_4                      0b11




// REG_PA_LEVEL functions
#define PA0_ON(N)                       ((N) << 7)
#define PA1_ON(N)                       ((N) << 6)
#define PA2_ON(N)                       ((N) << 5)
#define OUTPUT_POWER(N)                 ((N) << 0)

// PAX_ON arguments (already defined)
// OFF                                  0b0    // Default for PA2 and PA3
// ON                                   0b1    // Default for PA1



// REG_PA_RAMP functions
#define PA_RAMP(N)                      ((N) << 0)

// PA_RAMP arguments
#define RAMP_3400US                     0b0000
#define RAMP_2MS                        0b0001
#define RAMP_1MS                        0b0010
#define RAMP_500US                      0b0011
#define RAMP_250US                      0b0100
#define RAMP_125US                      0b0101
#define RAMP_100US                      0b0110
#define RAMP_62US                       0b0111
#define RAMP_50US                       0b1000
#define RAMP_40US                       0b1001
#define RAMP_31US                       0b1010
#define RAMP_25US                       0b1011
#define RAMP_20US                       0b1100
#define RAMP_15US                       0b1101
#define RAMP_12US                       0b1110
#define RAMP_10US                       0b1111




// REG_OCP functions
#define OCP_ON(N)                       ((N) << 4)
#define OCP_TRIM(N)                     ((N) << 0)

// OCP_ON arguments (already defined)
// OFF                                  0b0
// ON                                   0b1    // Default

// OCP_TRIM arguments
// Default byte         0b1010: 95mA
// Imax = 45 + 5 * OCPTrim (mA)




// REG_LNA functions
#define LNA_ZIN(N)                      ((N) << 7)
#define LNA_CURRENT_GAIN                (1   << 3)
#define LNA_GAIN_SELECT(N)              ((N) << 0)

// LNA_ZIN arguments
#define Z_50                            0b0
#define Z_200                           0b1    // Default

/**
 * @defgroup SX1231_GAIN
 * @brief LNA_GAIN_SELECT arguments
 * @{
 */
#define GAIN_0                          0b000  // AGC loop gain. Default
#define GAIN_1                          0b001  // Highest gain
#define GAIN_2                          0b010  // Highest gain - 6dB
#define GAIN_3                          0b011  // Highest gain - 12dB
#define GAIN_4                          0b100  // Highest gain - 24dB
#define GAIN_5                          0b101  // Highest gain - 36dB
#define GAIN_6                          0b110  // Highest gain - 48dB
/** @} */




// REG_BX_BW function
#define DCC_FREQ(N)                     ((N) << 5)
#define RX_BW_MANT(N)                   ((N) << 3)
#define RX_BW_EXP(N)                    ((N) << 0)

// DCC_FREQ arguments
// 3 bits see p.67 of the Datasheet
// Default value: 010

// RX_BW_MANT arguments
#define MANT_16                         0b00
#define MANT_20                         0b01
#define MANT_24                         0b10   // Default

// RX_BW_EXP arguments
// 3 bits see p.67 of the Datasheet
// Default value: 101




// REG_AFC_BW functions
#define DCC_FREQ_AFC(N)                 ((N) << 5)
#define RX_BW_MANT_AFC(N)               ((N) << 3)
#define RX_BW_EXP_AFC(N)                ((N) << 0)


// DCC_FREQ_AFC arguments
// 3 bits, see datasheet p.67. Default: 100

// RX_BW_MANT_AFC arguments
// 2 bits, default:01

// RX_BW_MANT_AFC arguments
// 3bits, default: 011



// REG_OOK_PEAK functions
#define OOK_THRESH_TYPE(N)              ((N) << 6)
#define OOK_PEAK_THRESH_STEP(N)         ((N) << 3)
#define OOK_PEAK_THRESH_DEC(N)          ((N) << 0)

// OOK_THRESH_TYPE arguments
#define OOK_TYPE_FIXED                  0b00
#define OOK_TYPE_PEAK                   0b01
#define OOK_TYPE_AVERAGE                0b10

// OOK_PEAK_THRESH_STEP arguments
#define OOK_STEP_0_5DB                  0b000
#define OOK_STEP_1DB                    0b001
#define OOK_STEP_1_5DB                  0b010
#define OOK_STEP_2DB                    0b011
#define OOK_STEP_3DB                    0b100
#define OOK_STEP_4DB                    0b101
#define OOK_STEP_5DB                    0b110
#define OOK_STEP_6DB                    0b111

// OOK_PEAK_THRESH_DEC arguments
#define DEC_1_PER_1                     0b000
#define DEC_1_EVERY_2                   0b001
#define DEC_1_EVERY_4                   0b010
#define DEC_1_EVERY_8                   0b011
#define DEC_2_EACH_1                    0b100
#define DEC_4_EACH_1                    0b101
#define DEC_8_EACH_1                    0b110
#define DEC_16_EACH_1                   0b111




// REG_OOK_AVG
#define OOK_AVERAGE_THRESH_FILT(N)      ((N) << 6);

// OOK_AVERAGE_THRESH_FILT arguments
#define FILT_32PI                       0b00
#define FILT_8PI                        0b01
#define FILT_4PI                        0b10
#define FILT_2PI                        0b11




// REG_AFC_FEI functions
#define FEI_DONE(N)                     ((N) << 6)
#define FEI_START(N)                    ((N) << 5)
#define AFC_AUTO_CLEAR_ON(N)            ((N) << 3)
#define AFC_AUTO_ON(N)                  ((N) << 2)
#define AFC_CLEAR(N)                    ((N) << 1)
#define AFC_START(N)                    ((N) << 0)

// FEI_DONE arguments
#define FEI_ON_GOING                    0b0    // Default
#define FEI_FINISHED                    0b0

// FEI_START arguments
// Use ON to start a FEI

// AFC_AUTO_CLEAR_ON arguments
// Usual ON / OFF arguments

// AFC_AUTO__ON arguments
// Usual ON / OFF arguments

// AFC_CLEAR
// Use ON to clear the AFCValue in RX mode

// AFC_START arguments
// Use ON to trigger an AFC




// REG_RSSI_CONFIG functions
#define RSSI_START(N)                   ((N) << 0)
#define RSSI_DONE                       (0b1 << 1)
// RSSI_START arguments
// Use ON to trigger an RSSI sampling




// REG_DIO_MAPPING_1 and REG_DIO_MAPPING_2 functions
#define DIO0_MAPPING(N)                 ((N) << 6)
#define DIO1_MAPPING(N)                 ((N) << 4)
#define DIO2_MAPPING(N)                 ((N) << 2)
#define DIO3_MAPPING(N)                 ((N) << 0)

#define DIO4_MAPPING(N)                 ((N) << 6)
#define DIO5_MAPPING(N)                 ((N) << 4)
#define CLK_OUT(N)                      ((N) << 0)

// DIOx_MAPPING arguments
// See p.48 of the datasheet for details.
// Arguments is 2 bits.
// Default      0b00

// REG_IRQ_FLAGS_1 reading functions
#define FLAG_1_MODE_READY               (1 << 7)
#define FLAG_1_RX_READY                 (1 << 6)
#define FLAG_1_TX_READY                 (1 << 5)
#define FLAG_1_PLL_LOCK                 (1 << 4)
#define FLAG_1_RSSI                     (1 << 3)
#define FLAG_1_TIMEOUT                  (1 << 2)
#define FLAG_1_AUTO_MODE                (1 << 1)
#define FLAG_1_SYNC_ADDRESS             (1 << 0)

// REG_IRQ_FLAGS_2 reading functions
#define FLAG_2_FIFO_FULL                (1 << 7)
#define FLAG_2_FIFO_NOT_EMPTY           (1 << 6)
#define FLAG_2_FIFO_LEVEL               (1 << 5)
#define FLAG_2_FIFO_OVERRUN             (1 << 4)
#define FLAG_2_PACKET_SENT              (1 << 3)
#define FLAG_2_PAYLOAD_READY            (1 << 2)
#define FLAG_2_CRC_OK                   (1 << 1)
#define FLAG_2_LOW_BAT                  (1 << 0)


// CLK_OUT functions
// /!\ CLK_OUT_FX_OSC_x means FX_OSC/x
#define CLK_OUT_FX_OSC                  0b000
#define CLK_OUT_FX_OSC_2                0b001
#define CLK_OUT_FX_OSC_4                0b010
#define CLK_OUT_FX_OSC_8                0b011
#define CLK_OUT_FX_OSC_16               0b100
#define CLK_OUT_FX_OSC_32               0b101
#define CLK_OUT_RC                      0b110
#define CLK_OUT_OFF                     0b111  // Default

// REG_SYNC_CONFIG functions
#define SYNC_ON(N)                      ((N) << 7)
#define FIFO_FILL_CONDITION(N)          ((N) << 6)
#define SYNC_SIZE(N)                    ((N) << 3)
#define SYNC_TOL(N)                     ((N) << 0)

// SYNC_ON arguments (already defined)
// OFF                                  0b0   // Default
// ON                                   0b1

// FIFO_FILL_CONDITION arguments
// see p.71 of the datasheet

// SYNC_SIZE arguments
// On 3 bits
// The actual size defined is Sync_Size + 1 bytes
// Default:      0b011

// SYNC_TOL arguments
// On 3 bits
// Number of tolerated bit errors in Sync word
// Default:      0b000




// REG_PACKET_CONFIG_1 functions
#define PACKET_FORMAT(N)                ((N) << 7)
#define DC_FREE(N)                      ((N) << 5)
#define CRC_ON(N)                       ((N) << 4)
#define CRC_AUTO_CLEAR_OFF(N)           ((N) << 3)
#define ADDRESS_FILTERING(N)            ((N) << 1)

// PACKET_FORMAT arguments
#define FIXED_LENGTH                    0b0   // Default
#define VARIABLE_LENGTH                 0b1

// DC_FREE arguments
#define DC_NONE                         0b00  // Default
#define DC_MANCHESTER                   0b01
#define DC_WHITENING                    0b10

// CRC_ON arguments (already defined)
//      ON                              0b1   // Default
//      OFF                             0b0

// CRC_AUTO_CLEAR_OFF arguments (already defined)
// /!\ ON means that you confirm the OFF!
//      ON                              0b1
//      OFF                             0b0   // Default

// ADDRESS_FILTERING arguments
#define ADDR_FILT_NONE                  0b00
#define ADDR_FILT_NODE_ONLY             0b01
#define ADDR_FILT_NODE_BROADCAST        0b10





// REG_AUTO_MODES functions
#define ENTER_CONDITION(N)              ((N) << 5)
#define EXIT_CONDITION(N)               ((N) << 2)
#define INTERMEDIATE_MODE(N)            ((N) << 0)

// ENTER_CONDITION arguments
#define ENTER_CONDTION_NONE             0b000
#define ENTER_CONDTION_FIFO_NOT_EMPTY   0b001
#define ENTER_CONDTION_FIFO_LEVEL       0b010
#define ENTER_CONDTION_CRC_OK           0b011
#define ENTER_CONDTION_PAYLOAD_READY    0b100
#define ENTER_CONDTION_SYNC_ADDRESS     0b101
#define ENTER_CONDTION_PACKET_SENT      0b110
#define ENTER_CONDTION_FIFO_EMPTY       0b111

// EXIT_CONDITION arguments
#define EXIT_CONDTION_NONE              0b000
#define EXIT_CONDTION_FIFO_EMPTY        0b001
#define EXIT_CONDTION_FIFO_LEVEL        0b010
#define EXIT_CONDTION_CRC_OK            0b011
#define EXIT_CONDTION_PAYLOAD_READY     0b100
#define EXIT_CONDTION_SYNC_ADDRESS      0b101
#define EXIT_CONDTION_PACKET_SENT       0b110
#define EXIT_CONDTION_TIME_OUT          0b111

// INTERMEDIATE_MODE arguments (already defined)
//      SLEEP                           0b000
//      STDBY                           0b001  // Default
//      FS                              0b010
//      TX                              0b011
//      RX                              0b100




// REG_FIFO_THRESH functions
#define TX_START_CONDITION(N)           ((N) << 7)
#define FIFO_THRESHOLD(N)               ((N) << 0)

// TX_START_CONDITION
#define TX_CONDITION_FIFO_LEVEL         0b0
#define TX_CONDITION_FIFO_NOT_EMPTY     0b1

// FIFO_THRESHOLD
// 7 bits data
// Default: 0b0001111


// REG_PACKET_CONFIG_2 functions
#define INTER_PACKET_RX_DELAY(N)        ((N) << 4)
#define RESTART_RX(N)                   ((N) << 2)
#define AUTO_RX_RESTART_ON(N)           ((N) << 1)
#define AES_ON(N)                       ((N) << 0)

// INTER_PACKET_RX_DELAY arguments
// 4-bits data
// Default: 0b0000
// See p.73 of the datasheet

// AUTO_RX_RESTART_ON arguments (already defined)
//      ON                              0b1   // Default
//      OFF                             0b0

// AES_ON arguments (already defined)
//      ON                              0b1
//      OFF                             0b0   // Default



// REG_TEMP_1 functions
#define TEMP_MEAS_START(N)              ((N) << 3)

// TEMP_MEAS_START arguments
// Use ON to trigger a measure of the temperature


//==========================================================================//
// Functions                                                                //
//==========================================================================//

/************************************ SPI ***********************************/

/**
 * @brief Reset the SX1231 and initialised the SPI for the SX1231,
 */
void sx_init(void);

/**
 * @brief Read in burst the register of the SX1231.
 *
 * @note The burst reading works on every register except REG_FIFO. If you do
 * a burst from REG_FIFO you will read <size> times the FIFO.
 *
 * @param address  First address to be read.
 * @param data     Pointer of the buffer where the read value will be stocked.
 * @param size     Number of register to read
 */
void sx_read(uint8_t address, void *data, size_t size);

/**
 * @brief Make reading access with ignoring what is returned by the SX1231.
 *
 * @param address  First address to be read.
 * @param data     Pointer of the buffer where the read value will be stocked.
 * @param size     Number of register to read
 */
void sx_read_dummy(uint8_t address, size_t size);

/**
 * @brief Write in burst in the register of the SX1231.
 *
 * @note The burst writing works on every register except REG_FIFO. If you do
 * a burst from REG_FIFO you will write n times in the FIFO.
 *
 * @param address  First address in which we will write.
 * @param data     Pointer of the buffer containing the data to write.
 * @param size     Number of register to write
 */
void sx_write(uint8_t address, const void *data, size_t size);

/**
 * @brief Read a byte/register of the SX1231.
 *
 * @param address  The address of the register to read.
 *
 * @return The read byte.
 */
uint8_t sx_read8(uint8_t address);

/**
 * @brief Write a byte in a register of the SX1231.
 *
 * @param address  The address of the register to write in.
 * @param data     The byte to write.
 */
void sx_write8(uint8_t address, uint8_t data);

/**
 * @brief Write two bytes in two consecutive registers of the SX1231.
 *
 * @param address  The address of the first register where we will write.
 * @param data     The bytes to write.
 */
void sx_write16(uint8_t address, uint16_t data);

/**
 * @brief Write three bytes in three conecutive registers of the SX1231.
 *
 * @param address  The address of the register to read.
 * @param data     The bytes to write.
 */
void sx_write24(uint8_t address, uint32_t data);

/************************ SX1231 Settings functions *************************/
/**
 * @brief Put the SX1231 in the selected mode.
 *
 * @param mode SX1231_MODE
 */
void sx_mode(uint8_t mode);

/**
 * @brief Set the value of the RSSI threshold.
 *
 * @param rssi_thresh  Value of the rssi threshold to be setted.
 * Between 0 and 66.
 */
void set_rssi_thresh(uint8_t rssi_thresh);

/**
 * @brief Set the gain of the SX1231 in transmission.
 *
 * @param gain   See LNA_GAIN_SELECT arguments above.
 */
void set_gain(uint8_t gain);

/***************************** SX1231 functions *****************************/

/**
 * @brief Use the SX1231 functions to scan the current value of RSSI.
 * @return The value of RSSI scanned.
 */
uint8_t scan_rssi(void);

/**
 * @brief Empty the FIFO of the SX1231.
 */
void empty_fifo(void);

/************************ Communication functions ***************************/
// Configuration value.
#define VALUE_FIFO_THRESH 33

/**
 * @defgroup SX1231_ERR
 * @{
 */
#define TX_SUCCESS         0
#define TIMEOUT           -1 /**< Timeout occurred, no packet received */
#define RECEIVE_CRC_ERROR -2 /**< The packet received is corrupt */
#define RECEIVE_TOO_LONG  -3 /**< The packet received is larger than
                              * expected */
#define CHANNEL_BUSY      -4 /**< Someone else is emmitting on the channel */
/** @} */

/**
 * @brief Send a packet through radio.
 *
 * @param packet    Pointer to the buffer containing the data to send.
 * @param size      Size of the data to sned. Cannot be up to 255.
 * @param timeout   Time during which we try to send a message.
 *
 * @return SX1231_ERR
 */
int send_packet(const void *packet, size_t size, systime_t timeout);

/**
 * @brief Pass the SX1231 in RX mode and try to catch a radio packet.
 *
 * @param  packet            Buffer where the packet received will be stocked.
 * @param  max_size          Maximum size allowed for receiving a packet.
 * @param  timeout           Time during which we wait for a packet.
 *
 * @return The size of the packet, if the packet is correct or SX1231_ERR
 */
int receive_packet(void *packet, size_t max_size, systime_t timeout);

#endif //__SX1231_H__
