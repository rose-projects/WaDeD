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

#include "waded_usb.h"
#include "client_cmd.h"
#include "string_handler.h"

uint16_t       host_id    = 0xFFFF;
static uint8_t usb_active = 0;

#define N_CMD 9

/* WARNING: The command at position i must have the ID i+1 */
static char *cmd_list[] = {
    SEND_TXT_CMD,
    SET_ID_CMD,
    PING_CMD,
    ACK_CMD,
    NACK_CMD,
    REC_TXT_CMD,
    GET_ID_CMD,
    MY_ID_CMD,
    SET_DATE_CMD
};

//USE_MEMORY
char cmd_buf[CMD_BUF_SIZE];

#define cmd_buf_in cmd_buf

/**
 * @brief Verify if a given command is in the start of a given string.
 *
 * @param cmd The input command.
 * @param str The string to compare with.
 *
 * @return 0 if the comparaison is negatif, or a pointer to after the command
 * in the string.
 */
static char * has_command(const char *cmd, char *str)
{
    while ((*cmd == *str) && *cmd) {
        cmd++;
        str++;
    }
    if (!(*cmd))
        return str;
    else
        return 0;
}

/**
 * @brief Returns the ID of the command and make cmd_ready point to after
 * the command in the string.
 *
 * @param cmd       The command.
 * @param cmd_ready Pointer to be placed after the command in the string.
 *
 * @return The command ID.
 */
static uint16_t get_command_id(char *cmd, char ** const cmd_ready)
{
    char *my_cmd = skip_spaces(cmd);

    if (is_num(*my_cmd))
        return str_to_int(my_cmd, cmd_ready);
    else {
        for (int i = 0; i < N_CMD; i++) {
            char *aux_cmd = has_command(cmd_list[i], my_cmd);
            if (aux_cmd) {
                if (cmd_ready)
                    *cmd_ready = aux_cmd;
                return i + 1;
            }
        }
        return 0;
    }
}

/* if fails return 1, else 0
   this function acts like the string starts AFTER the command word*/
static uint16_t send_txt_from_str(char *str, struct Bucket *buc)
{
    str = skip_spaces(str);
    if(!is_num(*str))
            return 1;
    buc->destination_address = str_to_int(str, &str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;

    string_copy((char *) buc->message , str, 0);
    return 0;
}

static uint16_t set_date_from_str(char *str,
                __attribute__((unused)) struct Bucket *buc)
{
    uint16_t year, month, day, hour, minute, second;

    str = skip_spaces(str);
    if(!is_num(*str))
            return 1;
    year = str_to_int(str, &str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;
    str = skip_spaces(str);

    if(!is_num(*str))
            return 1;
    month = str_to_int(str,&str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;
    str = skip_spaces(str);

    if(!is_num(*str))
            return 1;
    day = str_to_int(str,&str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;
    str = skip_spaces(str);

    if(!is_num(*str))
            return 1;
    hour = str_to_int(str,&str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;

    if(!is_num(*str))
            return 1;
    minute = str_to_int(str,&str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;
    str = skip_spaces(str);

    if(!is_num(*str))
            return 1;
    second = str_to_int(str,&str);
    str = skip_spaces(str);
    if(is_end(*str))
            return 1;

    if(!set_timestamp(year, month, day, hour, minute, second))
            return 1;

    return 0;
}

static uint16_t set_id_from_str(char *str, struct Bucket *buc)
{
    str = skip_spaces(str);
    if(!is_num(*str))
            return 1;
    buc->source_address = str_to_int(str, &str);

    return 0;
}

static char* my_id_to_str(char *str, uint16_t id)
{
    uint16_t i;

    /* set the command word */
    i = string_copy(str, MY_ID_CMD, 0);
    str += i;
    *(str++) = ' ';

    /* set the source ID */
    str = int_to_str(str, id);
    str = insert_linefeed(str);
    return str;
}

static char* ack_to_str(char *str)
{
    uint16_t i;

    /* set the command word */
    i = string_copy(str, ACK_CMD, 0);
    str += i;
    str = insert_linefeed(str);
    return str;
}

static char* nack_to_str(char *str, const char *error)
{
    /* set the command word */
    str += string_copy(str, NACK_CMD, 0);
    *(str++) = ' ';

    str += string_copy(str, error, 0);
    str = insert_linefeed(str);
    return str;
}

/**
 * @brief Signal an error trhough USB.
 *
 * @param str   The command where the error occured.
 * @param error The error.
 */
static inline void send_nack(char *str, const char *error)
{
    nack_to_str(str, error);
    usb_puts(str);
}

/**
 * @brief Send an acknoledge through USB for a given command.
 *
 * @param str The command in string.
 */
static inline void send_ack(char *str)
{
    ack_to_str(str);
    usb_puts(str);
}

/**
 * @brief Store an incomming USB command in cmd_buf_in.
 */
static void usb_get_command(void)
{
    for (int i = 0; i < (CMD_BUF_SIZE - 1); i++) {
        cmd_buf_in[i] = usb_get();
        if (cmd_buf_in[i] == '\n' || cmd_buf_in[i] == '\r') {
            cmd_buf_in[i] = '\0';
            break;
        }
    }
}

/**
 * @brief Transform a Bucket struct into a readable string.
 *
 * @param str The output string.
 * @param buc The input bucket.
 */
static void bucket_to_str(char *str, const struct Bucket *buc)
{
    // Prefix with the command.
    str += string_copy(str, REC_TXT_CMD, 0);
    *(str++) = ' ';

    // Set the source ID.
    str = int_to_str(str,buc->source_address);
    *(str++) = ' ';

    // Copy the message itself.
    str += string_copy(str, (char *) buc->message, 0);
    str = insert_linefeed(str);
}

uint16_t get_command(struct Bucket * buc)
{
    usb_get_command();

    char *content;
    uint16_t id = get_command_id(cmd_buf_in, &content);

    switch (id) {
        case SEND_TXT_ID:
            if (send_txt_from_str(content, buc)) {
                send_nack(cmd_buf, "bad command");
                return 0;
            }
            send_ack(cmd_buf);
            return SEND_TXT_ID;
        case SET_ID_ID:
            if (set_id_from_str(content, buc)) {
                send_nack(cmd_buf, "bad command");
                return 0;
            }
            send_ack(cmd_buf);
            return SET_ID_ID;
        case PING_ID:
            send_ack(cmd_buf);
            return 0;
        case GET_ID_ID:
            return GET_ID_ID;
        case SET_DATE_ID:
            if (!set_date_from_str(content, buc))
                send_nack(cmd_buf, "bad command");
            else
                send_ack(cmd_buf);
            return 0;
        default:
            send_nack(cmd_buf, "command unknown");
            return 0;
    }
}

void send_usr_id(uint16_t id)
{
    my_id_to_str(cmd_buf, id);
    usb_puts(cmd_buf);
}

void send_usr_message(const struct Bucket *buc)
{
    if (!usb_active || buc->destination_address != host_id)
        return;
    bucket_to_str(cmd_buf, buc);
    usb_puts(cmd_buf);
}

void activate_usb(void)
{
    for(int count = 0; count < 3;) {
        usb_get_command();
        if (get_command_id(cmd_buf_in, 0) == PING_ID) {
            count++;
            send_ack(cmd_buf);
        } else {
            count = 0;
            send_nack(cmd_buf, "please ping");
        }
    }
    usb_active = 1;
}
