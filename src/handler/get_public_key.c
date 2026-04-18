/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "cx.h"
#include "os.h"
#include "buffer.h"
#include "crypto_helpers.h"
#include "io.h"

#include "get_public_key.h"
#include "globals.h"
#include "sw.h"
#include "display.h"
#include "send_response.h"
#include "constants.h"
#include "crypto/path_policy.h"

int handler_get_public_key(buffer_t *cdata, bool display, uint8_t chain_id) {
    explicit_bzero(&G_context, sizeof(G_context));
    G_context.chain_id = chain_id;

    uint8_t levels = 0;
    if (!buffer_read_u8(cdata, &levels) || levels != WAVES_BIP32_DEPTH) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_WRONG_DATA_LENGTH);
    }
    if (!buffer_read_bip32_path(cdata, G_context.bip32_path, WAVES_BIP32_DEPTH)) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_WRONG_DATA_LENGTH);
    }
    if (!waves_bip32_path_valid(G_context.bip32_path, WAVES_BIP32_DEPTH)) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_INCORRECT_DATA);
    }
    G_context.bip32_path_len = WAVES_BIP32_DEPTH;

    cx_ecfp_256_private_key_t priv;
    cx_ecfp_256_public_key_t pub;
    explicit_bzero(&priv, sizeof(priv));
    explicit_bzero(&pub, sizeof(pub));

    cx_err_t err = bip32_derive_with_seed_init_privkey_256(HDW_ED25519_SLIP10,
                                                           CX_CURVE_Ed25519,
                                                           G_context.bip32_path,
                                                           G_context.bip32_path_len,
                                                           &priv,
                                                           NULL,
                                                           NULL,
                                                           0);
    if (err != CX_OK) {
        explicit_bzero(&priv, sizeof(priv));
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_SECURITY_ISSUE);
    }

    err = cx_eddsa_get_public_key_no_throw(&priv, CX_SHA512, &pub, NULL, 0, NULL, 0);
    explicit_bzero(&priv, sizeof(priv));
    if (err != CX_OK) {
        explicit_bzero(&pub, sizeof(pub));
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_SECURITY_ISSUE);
    }
    /* lib_cxng: Ed25519 pubkey is 0x04 + y + x (W_len 65); legacy layout was 33 bytes. */
    if (pub.W_len == 65 && pub.W[0] == 0x04) {
        err = cx_edwards_compress_point_no_throw(CX_CURVE_Ed25519, pub.W, pub.W_len);
        if (err != CX_OK) {
            explicit_bzero(&pub, sizeof(pub));
            explicit_bzero(&G_context, sizeof(G_context));
            return io_send_sw(SWO_SECURITY_ISSUE);
        }
        memcpy(G_context.pk_info.public_key, pub.W + 1, WAVES_PUBKEY_LEN);
    } else if (pub.W_len == 33) {
        memcpy(G_context.pk_info.public_key, pub.W + 1, WAVES_PUBKEY_LEN);
    } else {
        explicit_bzero(&pub, sizeof(pub));
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_SECURITY_ISSUE);
    }
    explicit_bzero(&pub, sizeof(pub));

    if (display) {
        G_context.state = STATE_CONFIRM_ADDRESS;
        G_context.req_display_address = 1;
        return ui_display_address();
    }

    return helper_send_response_pubkey();
}
