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

#include "string_handler.h"

char * skip_spaces(char *str)
{
    for (; *str == ' '; str++) ;
    return str;
}

/* this functions returns an int from a string and place str_ready at the end
 * of the value */
uint16_t str_to_int(char *str, char ** const str_ready){
    uint16_t value = 0;

    while (is_num(*str)){
        value *= 10;
        value += *str - '0';
        str++;
    }
    if (str_ready)
        *str_ready = str;

    return value;
}

/* returns the position right after the string if filled */
char* int_to_str(char *str, uint32_t value){
    uint16_t i;
    uint16_t j;
    char aux;

    for (i = 0; value; i++) {
        str[i] = (value % 10) + '0';
        value /= 10;
    }
    str[i] = '\0';
    for (j = 0; j < i / 2; j++) {
        aux = str[j];
        str[j] = str[i - (j + 1)];
        str[i - (j + 1)] = aux;
    }

    return &str[i];
}

#define MAX_HIGH_VALUE 1000
uint16_t string_copy(char * dest, const char * src, uint16_t max){
    uint16_t i;
    if (max == 0)
        max = MAX_HIGH_VALUE;
    for (i = 0; i != max; i++){
        dest[i] = src[i];
        if (is_end(src[i])) {
            dest[i] = '\0';
            break;
        }
    }
    return i;
}

char* insert_linefeed(char *str)
{
    *(str++) = '\n';
    *(str++) = '\r';
    *str     = '\0';
    return str;
}
