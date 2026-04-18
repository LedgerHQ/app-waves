/*
 * WAVES Ledger Application — address encoding.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "address.h"

#include <stdio.h>
#include <string.h>

#include "cx.h"
#include "os.h"
#include "base58.h"

#include "crypto/secure_hash.h"

#define WAVES_ADDR_VERSION 0x01

bool waves_encode_address(uint8_t chain_id,
                          const uint8_t pubkey[WAVES_PUBKEY_LEN],
                          char *out,
                          size_t out_sz) {
    if (out == NULL || out_sz < WAVES_ADDRESS_STR_MAX) {
        return false;
    }

    explicit_bzero(out, out_sz);

    uint8_t h32[32];
    if (waves_secure_hash_one_shot(pubkey, WAVES_PUBKEY_LEN, h32) != CX_OK) {
        explicit_bzero(h32, sizeof(h32));
        return false;
    }

    uint8_t body[22];
    body[0] = WAVES_ADDR_VERSION;
    body[1] = chain_id;
    memcpy(body + 2, h32, 20);

    uint8_t chk_input[22];
    memcpy(chk_input, body, sizeof(chk_input));
    uint8_t chk_hash[32];
    if (cx_keccak_256_hash(chk_input, sizeof(chk_input), chk_hash) != CX_OK) {
        explicit_bzero(h32, sizeof(h32));
        explicit_bzero(chk_hash, sizeof(chk_hash));
        return false;
    }

    uint8_t raw[26];
    memcpy(raw, body, 22);
    memcpy(raw + 22, chk_hash, 4);
    explicit_bzero(h32, sizeof(h32));
    explicit_bzero(chk_hash, sizeof(chk_hash));

    int enc = base58_encode(raw, sizeof(raw), out, out_sz);
    explicit_bzero(raw, sizeof(raw));
    if (enc < 0) {
        return false;
    }
    return true;
}

void waves_format_network_line(uint8_t chain_id, char *buf, size_t buf_sz) {
    if (buf == NULL || buf_sz == 0) {
        return;
    }
    switch (chain_id) {
        case 0x57:
            (void) snprintf(buf, buf_sz, "%s", "Network: Mainnet");
            break;
        case 0x54:
            (void) snprintf(buf, buf_sz, "%s", "Network: Testnet");
            break;
        case 0x53:
            (void) snprintf(buf, buf_sz, "%s", "Network: Stagenet");
            break;
        default:
            (void) snprintf(buf, buf_sz, "Network: Custom (0x%02X)", chain_id);
            break;
    }
}
