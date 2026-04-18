/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "cx.h"
#include "os.h"
#include "crypto_helpers.h"
#include "io.h"

#include "validate.h"
#include "globals.h"
#include "sw.h"
#include "send_response.h"
#include "constants.h"

void validate_pubkey(bool choice) {
    if (choice) {
        (void) helper_send_response_pubkey();
    } else {
        explicit_bzero(&G_context, sizeof(G_context));
        (void) io_send_sw(SWO_CONDITIONS_NOT_SATISFIED);
    }
}

void validate_blind_sign(bool choice) {
    if (G_context.state != STATE_SIGN_STREAM_UI) {
        explicit_bzero(&G_context, sizeof(G_context));
        (void) io_send_sw(SWO_CONDITIONS_NOT_SATISFIED);
        return;
    }

    if (!choice) {
        explicit_bzero(&G_context, sizeof(G_context));
        (void) io_send_sw(SWO_CONDITIONS_NOT_SATISFIED);
        return;
    }

    size_t sig_len = sizeof(G_context.sign.signature);
    cx_err_t err = bip32_derive_with_seed_eddsa_sign_hash_256(HDW_ED25519_SLIP10,
                                                               CX_CURVE_Ed25519,
                                                               G_context.bip32_path,
                                                               G_context.bip32_path_len,
                                                               CX_SHA512,
                                                               G_context.sign.secure_hash,
                                                               sizeof(G_context.sign.secure_hash),
                                                               G_context.sign.signature,
                                                               &sig_len,
                                                               NULL,
                                                               0);

    if (err != CX_OK || sig_len != WAVES_SIG_LEN) {
        explicit_bzero(&G_context, sizeof(G_context));
        (void) io_send_sw(SWO_SECURITY_ISSUE);
        return;
    }

    (void) helper_send_response_signature_ed25519();
    explicit_bzero(&G_context, sizeof(G_context));
}
