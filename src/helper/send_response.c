/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "io.h"
#include "os.h"

#include "send_response.h"
#include "address.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"

int helper_send_response_pubkey(void) {
    char addr[WAVES_ADDRESS_STR_MAX];
    uint8_t resp[WAVES_PUBKEY_LEN + 1 + WAVES_ADDRESS_STR_MAX];
    int ret;

    explicit_bzero(addr, sizeof(addr));

    if (!waves_encode_address(G_context.chain_id,
                              G_context.pk_info.public_key,
                              addr,
                              sizeof(addr))) {
        explicit_bzero(addr, sizeof(addr));
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_INCORRECT_DATA);
    }

    size_t alen = strnlen(addr, sizeof(addr));
    if (alen == 0 || alen > UINT8_MAX) {
        explicit_bzero(addr, sizeof(addr));
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_INCORRECT_DATA);
    }

    size_t off = 0;
    memcpy(resp + off, G_context.pk_info.public_key, WAVES_PUBKEY_LEN);
    off += WAVES_PUBKEY_LEN;
    resp[off++] = (uint8_t) alen;
    memcpy(resp + off, addr, alen);
    off += alen;

    ret = io_send_response_pointer(resp, off, SWO_SUCCESS);
    explicit_bzero(resp, sizeof(resp));
    explicit_bzero(addr, sizeof(addr));
    explicit_bzero(&G_context, sizeof(G_context));
    return ret;
}

int helper_send_response_signature_ed25519(void) {
    return io_send_response_pointer(G_context.sign.signature, WAVES_SIG_LEN, SWO_SUCCESS);
}
