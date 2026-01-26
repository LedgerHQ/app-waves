// /*****************************************************************************
//  *   Ledger App Boilerplate.
//  *   (c) 2020 Ledger SAS.
//  *
//  *  Licensed under the Apache License, Version 2.0 (the "License");
//  *  you may not use this file except in compliance with the License.
//  *  You may obtain a copy of the License at
//  *
//  *      http://www.apache.org/licenses/LICENSE-2.0
//  *
//  *  Unless required by applicable law or agreed to in writing, software
//  *  distributed under the License is distributed on an "AS IS" BASIS,
//  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  *  See the License for the specific language governing permissions and
//  *  limitations under the License.
//  *****************************************************************************/

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"
#include "buffer.h"
#include "swap.h"
#include "sign.h"
#include "show_sign_ui.h"

#include "ui/display/sign.h"
#include "sign_utils.h"
#include "sw.h"
#include "globals.h"
#include "display.h"
#include "tx_types.h"
#include "send_response.h"

int handler_sign(buffer_t *cdata, uint8_t p1, uint8_t p2) {
    cx_err_t error = CX_OK;

    if (G_context.signing_context.step <= 5) {
        if (G_context.signing_context.step > 0) {
            G_context.signing_context.chunk += 1;
        } else {
            PRINTF("make_sign_steps start\n");
            wait_data_spinner();
            G_context.signing_context.step = 1;
            G_context.signing_context.network_byte = p2;
        }

        G_context.signing_context.chunk_used = 0;
        error = make_allowed_sign_steps(cdata);

        if (error != CX_OK) {
            return io_send_sw(error);
        }

        if (G_context.signing_context.step == 5) {
            int len = 0;
            error = stream_eddsa_sign_step5(&G_context.signing_context.eddsa_context,
                                            G_context.signing_context.signature,
                                            &len);
            if (error != CX_OK) {
                return io_send_sw(error);
            }
            G_context.signing_context.step = 6;
            PRINTF("make_sign_steps end\n");
            memset(&G_context.signing_context.ui, 0, sizeof(G_context.signing_context.ui));
            error = cx_blake2b_init_no_throw(&G_context.signing_context.ui.hash_ctx, 256);
            if (error != CX_OK) {
                return io_send_sw(error);
            }
        } else {
            return io_send_sw(SW_OK);
            // wait for next chunk for sign
        }
    }

    if (p1 == P1_LAST) {
        PRINTF("make_allowed_ui_steps LAST\n");
        error = make_allowed_ui_steps(cdata, true);
        if (error != CX_OK) {
            return io_send_sw(error);
        }

        if (G_context.signing_context.step != 8) {
            return io_send_sw(SW_DEPRECATED_SIGN_PROTOCOL);
        }
    } else {
        PRINTF("make_allowed_ui_steps NEXT\n");
        error = make_allowed_ui_steps(cdata, false);
        if (error != CX_OK) {
            return io_send_sw(error);
        }
        PRINTF("make_allowed_ui_steps Step:=%02X | Size:=%02X. Used:=%02X \n",
               G_context.signing_context.step,
               G_context.signing_context.data_size,
               G_context.signing_context.chunk_used);
        return io_send_sw(SW_OK);
    }

    // all data parsed and prepeared to view
    if (G_context.signing_context.step == 8) {
        size_t length = 45;
        if (!b58enc((char *) G_context.signing_context.first_data_hash,
                    &length,
                    (const void *) &G_context.signing_context.first_data_hash,
                    32)) {
            return io_send_sw(SW_CONDITIONS_NOT_SATISFIED);
        }
        // convert sender public key to address
        waves_public_key_to_address(G_context.signing_context.ui.from,
                                    G_context.signing_context.network_byte,
                                    G_context.signing_context.ui.from);
        // if transaction has from field and it is pubkey hash will convert to
        // address
        if (G_context.signing_context.message_type == PROTOBUF_DATA) {
            PRINTF("Approve PROTOBUF sign");
            if (show_sign_ui()) {
                return io_send_sw(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            }
        } else {
            PRINTF("Approve LEGACY sign");
            if (show_sign_ui()) {
                return io_send_sw(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            }
        }
    }
    return CX_OK;
}
