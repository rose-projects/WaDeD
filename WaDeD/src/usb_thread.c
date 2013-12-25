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

#include "usb_thread.h"
#include "client_cmd.h"
#include "string_handler.h"
#include <string.h>

#define DAY_MS (24 * 60 * 60 * 1000)

//USE_MEMORY
static struct Bucket usb_buc;

/**
 * @brief Set a bucket hash.
 * @param buc Pointer to the Bucket (other fields must be already set)
 */
uint64_t bucket_hash(struct Bucket *buc){
    uint16_t *b2_aux;

    b2_aux = (uint16_t *) cmd_buf;
    *b2_aux = host_id;
    b2_aux++;
    *b2_aux = buc->destination_address;
    b2_aux++;
    string_copy((char *) b2_aux, (char *) buc->message, 0);
    return hash(cmd_buf,
                    (2 * sizeof(uint16_t) + strlen((char*) buc->message)));
}

/**
 * @brief Send the user all messages destined to him.
 */
static void send_all_msgs(void){
    uint16_t pos = 0;

    while (1){
        pos = next_message(host_id,pos,&usb_buc);
        if (usb_buc.state)
            send_usr_message(&usb_buc);
        else break;
        pos++;
    }
}

static WORKING_AREA(WA_usb,(1024));

msg_t usb_thread(void *arg){
    (void) arg;

    activate_usb();
    while (1) {
        switch(get_command(&usb_buc)){
            case SET_ID_ID:
                host_id = usb_buc.source_address;
                send_all_msgs();
                break;
            case GET_ID_ID:
                send_usr_id(host_id);
                break;
            case SEND_TXT_ID:
                usb_buc.source_address = host_id;
                usb_buc.emission_date = get_timestamp();
                usb_buc.expiration_date = usb_buc.emission_date + DAY_MS;
                usb_buc.type.id = bucket_hash(&usb_buc);
                tree_insert(&usb_buc);
                break;
        }
    }

    return 0;
}

void usb_thread_init(void)
{
    chThdCreateStatic(WA_usb, sizeof WA_usb, NORMALPRIO, usb_thread, NULL);
}
