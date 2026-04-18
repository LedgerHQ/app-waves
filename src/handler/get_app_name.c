/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>

#include "io.h"

#include "get_app_name.h"
#include "constants.h"
#include "sw.h"

int handler_get_app_name(void) {
    _Static_assert(APPNAME_LEN < MAX_APPNAME_LEN, "APPNAME length");
    return io_send_response_pointer(PIC(APPNAME), APPNAME_LEN, SWO_SUCCESS);
}
