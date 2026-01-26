/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
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
 *****************************************************************************/

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"
#include "io.h"
#include "buffer.h"
#include "crypto_helpers.h"

#include "get_public_key.h"
#include "globals.h"
#include "types.h"
#include "sw.h"
#include "display.h"
#include "send_response.h"
#include "sign_utils.h"
#include "buffer_helper.h"


int handler_get_public_key(buffer_t *cdata, bool display, uint8_t chain_code) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.req_type = CONFIRM_ADDRESS;
    G_context.state = STATE_NONE;
    uint32_t path[5];

    if (!read_path_from_buffer(cdata, path, 5)) {
        return io_send_sw(SW_CONDITIONS_NOT_SATISFIED);
    }

    //return io_send_response_pointer(path, 5, SWO_SUCCESS);
    // Get the public key and return it.
    cx_ecfp_public_key_t public_key;

    cx_err_t error = get_curve25519_public_key_for_path(path, &public_key);

    if (error) {
        io_send_sw(SW_CONDITIONS_NOT_SATISFIED);
    }

    unsigned char address[35];
    error = waves_public_key_to_address(public_key.W, chain_code, address);
    
    if(error) {
      return io_send_sw(SW_CONDITIONS_NOT_SATISFIED);
    }

    memmove((char *)G_context.address_context.address, address, 35);
    G_context.address_context.address[35] = '\0';
    memmove((char *)G_context.address_context.public_key, public_key.W, 32);
    
    if (display) {
        return ui_display_address();
    }

    return helper_send_response_pubkey();
}
