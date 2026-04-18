/*
 * WAVES Ledger Application — BIP32 path policy (spec §2.2).
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "path_policy.h"

#include "constants.h"

#define BIP44_PURPOSE_HARDENED 0x8000002Cu

bool waves_bip32_path_valid(const uint32_t *path, size_t len) {
    if (path == NULL || len != WAVES_BIP32_DEPTH) {
        return false;
    }
    if (path[0] != BIP44_PURPOSE_HARDENED) {
        return false;
    }
    if (path[1] != WAVES_SLIP44_COIN_TYPE_HARDENED) {
        return false;
    }
    for (size_t i = 2; i < len; i++) {
        if ((path[i] & 0x80000000u) == 0u) {
            return false;
        }
    }
    return true;
}
