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

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t
#include <string.h>  // memmove

#include "buffer.h"

#include "send_response.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

//   memmove((char *)G_io_apdu_buffer,
//              (char *)tmp_ctx.address_context.public_key, 32);
//   memmove((char *)G_io_apdu_buffer + 32,
//              (char *)tmp_ctx.address_context.address, 35);

int helper_send_response_pubkey() {
    uint8_t resp[ADDRESS_LENGTH + WAVES_PUBKEY_LENGTH] = {0};
    memmove(resp, G_context.address_context.public_key, WAVES_PUBKEY_LENGTH);
    memmove(resp + WAVES_PUBKEY_LENGTH, G_context.address_context.address, ADDRESS_LENGTH);
    return io_send_response_pointer(resp, ADDRESS_LENGTH + WAVES_PUBKEY_LENGTH, SWO_SUCCESS);
}

int helper_send_response_sig() {
    uint8_t resp[1 + MAX_DER_SIG_LEN + 1] = {0};
    size_t offset = 0;

    resp[offset++] = 64;
    memmove(resp + offset, G_context.signing_context.signature, 64);
    offset += 64;
    resp[offset++] = (uint8_t) 80;

    return io_send_response_pointer(resp, offset, SWO_SUCCESS);
}

int set_result_sign() {
  G_context.signing_context.signature[63] |= G_context.signing_context.sign_bit;
  PRINTF("Signature:\n%.*H\n", 64, G_context.signing_context.signature);
  return io_send_response_pointer(G_context.signing_context.signature, 64, SWO_SUCCESS);
}