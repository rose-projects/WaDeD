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
 * @file  dash7.c
 * @brief Give all the functions to configure dash7 on the SX1231.
 * */
#include "dash7.h"

#include "sx1231.h"
#include "dio.h"
#include "global.h"

#define ROUND(x) ((uint32_t) ((x) + 0.5))

// Convert a frequency in MHz to its FRF/FDEV equivalent
#define FREQ(d) ROUND(((d) * 1e6) / (32e6 / (1 << 19)))

static const uint32_t center_frequencies[] = {
    FREQ(433.164), FREQ(433.272), FREQ(433.380), FREQ(433.488),
    FREQ(433.596), FREQ(433.704), FREQ(433.812), FREQ(433.920),
    FREQ(434.028), FREQ(434.136), FREQ(433.244), FREQ(434.352),
    FREQ(434.460), FREQ(434.568), FREQ(434.676)
};

static const uint32_t bandwidth[] = {
    FREQ(0.432 / 2), FREQ(0.216 / 2), FREQ(0.432 / 2), FREQ(0.648 / 2)
};

// Convert a symbol rate in kHz to its BITRATE equivalent
#define SYMRATE(s) ROUND(32e6 / ((s) * 1e3))

/**
 * @brief   Initialise the X1231, with the Dash7 options.
 *
 * @default The channel is the channel BASE. The mode is the FOREGROUND_MODE,
 * the fifo threshold is set at VALUE_FIFO_THRESH (defined in sx1231.h) and the
 * rssi threshold is set at VALUE_RSSI_THRESH (defind above).
 *
 * The operations realised are:
 * - Set the kind of modulation(Mode packet, modulation FSK, Gaussian filter,
 *   BT = 1.0).
 * - Select the frequency deviation: 0.50MHz.
 * - Select the Channel, (hi-rate channel by default).
 * - Select 50 ohms impedance for the out circuit. And use the maximum gain.
 * - Select the channel filter and DC cancellation parameters -
 *   83kHz & 206Hz (0.25%).
 * - Set the preamble to 4 bytes.
 * - Synchronisation word is set to two bytes.
 * - The packet mode is set by default to foreground packet.
 * - Packet format is variable size, check CRC, do not filter on address.
 * - We set the maximum payload length (255).
 * - We set the TX to start when the FIFO not empty. We set the FIFO threshold to
 *   VALUE_FIFO_THRESH (defined in sx1231.h).
 * - We set the RSSI threshold to VALUE_FIFO_THRESH (defined in dash7.h).
 */
void dash7_init(void)
{
    sx_write8(REG_DATA_MODUL, DATA_MODE(PACKET_MODE) | MODULATION_TYPE(FSK) |
              MODULATION_SHAPING(FILTER_1));

    sx_write16(REG_FDEV_MSB, FREQ(0.050));

    select_channel(HI_RATE_CHANNEL);

    sx_write8(REG_LNA, LNA_ZIN(0) | LNA_GAIN_SELECT(GAIN_0));

    // TODO: check if the default value (4%) is better or worse
    sx_write8(REG_RX_BW, RX_BW_MANT(MANT_24) | RX_BW_EXP(2) | DCC_FREQ(6));

    sx_write16(REG_PREAMBLE_MSB, 4);

    sx_write8(REG_SYNC_CONFIG, SYNC_ON(ON) | SYNC_SIZE(1));  // 1+1 = 2

    select_packet_mode(FOREGROUND_MODE);

    // TODO: the whitening may have to be done in software if we want to
    // use FEC
    sx_write8(REG_PACKET_CONFIG_1, PACKET_FORMAT(VARIABLE_LENGTH) |
              DC_FREE(DC_WHITENING) | CRC_ON(ON) | CRC_AUTO_CLEAR_OFF(ON) |
              ADDRESS_FILTERING(ADDR_FILT_NONE));

    sx_write8(REG_PAYLOAD_LENGTH, 255);

    sx_write8(REG_FIFO_THRESH,
              TX_START_CONDITION(TX_CONDITION_FIFO_NOT_EMPTY) |
              FIFO_THRESHOLD(VALUE_FIFO_THRESH));

    set_rssi_thresh(VALUE_RSSI_THRESH);
}

/**
 * @brief Select the designed channel.
 *
 * @param channel    Defined in dash7.h.
 */
void select_channel(uint8_t channel)
{
    // Select the base frequency
    sx_write24(REG_FRF_MSB, center_frequencies[channel & 0xf]);

    // Select the symbol rate (FSK 1.8 or FSK 0.5)
    uint8_t bandwidth_index = (channel >> 8) & 0x7;
    sx_write16(REG_BITRATE_MSB, SYMRATE(bandwidth_index <= 2 ? 55.555 : 200));
}

/**
 * @brief Select the packet mode
 *
 * @param mode     The packet mode. Background or foreground mode.
 */
void select_packet_mode(uint16_t mode)
{
    // TODO: check what the specifications says about the endianess
    sx_write16(REG_SYNC_VALUE_1, mode);
}
