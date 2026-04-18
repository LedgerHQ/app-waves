/*
 * WAVES Ledger Application — fuzz INIT payload (path + total size fields).
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "buffer.h"
#include "constants.h"
#include "crypto/path_policy.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    buffer_t buf = {.ptr = (uint8_t *) data, .size = size, .offset = 0};
    uint8_t levels = 0;
    uint32_t path[WAVES_BIP32_DEPTH];
    uint32_t total_be = 0;

    if (!buffer_read_u8(&buf, &levels)) {
        return 0;
    }
    if (levels != WAVES_BIP32_DEPTH) {
        return 0;
    }
    if (!buffer_read_bip32_path(&buf, path, WAVES_BIP32_DEPTH)) {
        return 0;
    }
    (void) waves_bip32_path_valid(path, WAVES_BIP32_DEPTH);
    (void) buffer_read_u32(&buf, &total_be, BE);
    return 0;
}
