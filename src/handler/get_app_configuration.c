/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "io.h"
#include "os.h"

#include "get_app_configuration.h"
#include "constants.h"
#include "sw.h"

int handler_get_app_configuration(void) {
    _Static_assert(APPNAME_LEN <= UINT8_MAX && APPNAME_LEN <= MAX_APPNAME_LEN, "APPNAME length");

    uint8_t resp[1 + 3 + 1 + MAX_APPNAME_LEN];
    size_t off = 0;

    resp[off++] = APP_CONFIG_FLAG_BLIND_SIGN;
    resp[off++] = (uint8_t) MAJOR_VERSION;
    resp[off++] = (uint8_t) MINOR_VERSION;
    resp[off++] = (uint8_t) PATCH_VERSION;

    resp[off++] = (uint8_t) APPNAME_LEN;
    memcpy(resp + off, APPNAME, APPNAME_LEN);
    off += APPNAME_LEN;

    int ret = io_send_response_pointer(resp, off, SWO_SUCCESS);
    explicit_bzero(resp, sizeof(resp));
    return ret;
}
