/*******************************************************************************
 *   Waves Platform Wallet App for Nano Ledger devices
 *   Copyright (c) 2017-2020 Sergey Tolmachev (Tolsi) <tolsi.ru@gmail.com>
 *
 *   Based on Sample code provided (c) 2016 Ledger and
 *                                 (c) 2017-2018 Jake B. (Burstcoin app)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/


#include <stdint.h>
#include <stdbool.h>

#include "buffer.h"
#include "io.h"
#include "ledger_assert.h"

#include "dispatcher.h"
#include "constants.h"
#include "globals.h"
#include "types.h"
#include "sw.h"
#include "get_version.h"
#include "get_app_name.h"
#include "get_public_key.h"
#include "sign.h"

        // CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | CData=%.*H",
        //        cmd.cla,
        //        cmd.ins,
        //        cmd.p1,
        //        cmd.p2,
        //        cmd.lc,
        //        cmd.lc,
        //        cmd.data);

int apdu_dispatcher(const command_t *cmd) {
    LEDGER_ASSERT(cmd != NULL, "NULL cmd");
    if (cmd->cla != CLA) {
        return io_send_sw(SWO_INVALID_CLA);
    }

    buffer_t buf = {0};

    switch (cmd->ins) {
        case GET_APP_VERSION:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }

            return handler_get_version();

        case GET_APP_NAME:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }

            return handler_get_app_name();

        case GET_PUBLIC_KEY:
            if (!cmd->data) {
                return io_send_sw(SWO_WRONG_DATA_LENGTH);
            }

            if (cmd->lc != 20) {
                return io_send_sw(SWO_WRONG_DATA_LENGTH);
            }

            buf.ptr = cmd->data;
            buf.size = cmd->lc;
            buf.offset = 0;

            return handler_get_public_key(&buf, (bool) cmd->p1, (uint8_t) cmd->p2);

         case INS_SIGN:
            PRINTF("APDU_DISPATCHER: %d\n", cmd->ins);
            if ((cmd->p1 != P1_MORE) &&(cmd->p1 != P1_LAST)) {
                io_send_sw(SW_INCORRECT_P1_P2);
            }
           
            if (!cmd->data) {
                return io_send_sw(SWO_WRONG_DATA_LENGTH);
            }
            buf.ptr = cmd->data;
            buf.size = cmd->lc;
            buf.offset = 0;

            return handler_sign(&buf, cmd->p1, cmd->p2);

         default:
             return io_send_sw(SWO_INVALID_INS);
    }
}
