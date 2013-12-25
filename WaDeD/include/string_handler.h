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
 * @file  string_handler.h
 * @brief Functions to handle strings.
 */

#ifndef __STRING_HANDLER_H__
#define __STRING_HANDLER_H__

#include <stdint.h>

uint16_t string_copy(char * dest, const char * src, uint16_t max);
char* int_to_str(char *str, uint32_t value);
uint16_t str_to_int(char *str, char ** const str_ready);
char* insert_linefeed(char *str);

/**
 * @brief Move a string pointer to the first non-space character.
 *
 * @param str The string.
 *
 * @return The pointer to the first non-space character.
 */
char * skip_spaces(char *str);

/**
 * @brief Tell if the char represents a numerical digit.
 *
 * @param c The char to test.
 *
 * @return 1 if is a numerical digit, or 0.
 */
static inline uint8_t is_num(char c)
{
    return ((c >= '0') && (c <= '9'));
}

/**
 * @brief Tell if a char represents the end of a string (\0, \n or \r).
 *
 * @param c The char to test.
 *
 * @return 1 if it represents the end, or 0.
 */
static inline uint8_t is_end(char c)
{
    return ((c == '\0') || (c == '\n') || (c == '\r'));
}

#endif // __STRING_HANDLER_H__
