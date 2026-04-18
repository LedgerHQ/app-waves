/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <limits.h>
#include <assert.h>

#include "io.h"

#include "get_version.h"
#include "constants.h"
#include "sw.h"

int handler_get_version(void) {
    _Static_assert(APPVERSION_LEN == 3, "version triplet");
    _Static_assert(MAJOR_VERSION >= 0 && MAJOR_VERSION <= UINT8_MAX, "major");
    _Static_assert(MINOR_VERSION >= 0 && MINOR_VERSION <= UINT8_MAX, "minor");
    _Static_assert(PATCH_VERSION >= 0 && PATCH_VERSION <= UINT8_MAX, "patch");

    const uint8_t vers[APPVERSION_LEN] = {
        (uint8_t) MAJOR_VERSION,
        (uint8_t) MINOR_VERSION,
        (uint8_t) PATCH_VERSION,
    };
    return io_send_response_pointer(vers, sizeof(vers), SWO_SUCCESS);
}
