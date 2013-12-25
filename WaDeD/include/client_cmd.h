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
 * @file  client_cmd.h
 * @brief Functions to communicate through USB with a client.
 */

#ifndef __CLIENT_CMD_H__
#define __CLIENT_CMD_H__

#include "stdint.h"
#include "bucket.h"
#include "timestamp.h"

#define UNDEF_ID 0
#define SEND_TXT_CMD "send_txt"
#define SEND_TXT_ID 1
#define SET_ID_CMD "set_id"
#define SET_ID_ID 2
#define PING_CMD "ping"
#define PING_ID 3
#define ACK_CMD "ack"
#define ACK_ID 4
#define NACK_CMD "nack"
#define NACK_ID 5
#define REC_TXT_CMD "rec_txt"
#define REC_TXT_ID 6
#define GET_ID_CMD "get_id"
#define GET_ID_ID 7
#define MY_ID_CMD "my_id"
#define MY_ID_ID 8
#define SET_DATE_CMD "set_date"
#define SET_DATE_ID 9

#define CMD_BUF_SIZE 170
extern char cmd_buf[];

extern uint16_t host_id;

/**
 * @brief  Get a command through USB and return its type.
 *
 * set_id puts the id in buc->source_address.
 * send_txt puts the id in destination_address and message in message.
 *
 * @param buc A bucket to store things.
 *
 * @return The command id.
 */
uint16_t get_command(struct Bucket *buc);

/**
 * @brief Send to the use the id.
 *
 * @param id the id to send.
 */
void send_usr_id(uint16_t id);

/**
 * @brief Send to the user a message destined to him.
 *
 * @param buc The bucket containing the message.
 */
void send_usr_message(const struct Bucket * buc);

/**
 * @brief Try to grab 3 pings before starting comunication.
 */
void activate_usb(void);


#endif // __CLIENT_CMD_H__

/*************************************
Commands list:
send_txt x string
    x is the id of the destination
    string is the message
set_id x
    x is the new id we'll answer by
ping
    answered by an ack
get_id
    ask for our id

Answers list:
ack
    confirmation
nack
    returned when error is received. oftenly followed by an error string
rec_txt x string
    x is the id of the source that sended the message
    string is the message
my_id x
    x is our id

*************************************/
