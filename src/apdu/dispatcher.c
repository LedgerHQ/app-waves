/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <stdbool.h>

#include "buffer.h"
#include "io.h"
#include "ledger_assert.h"

#include "dispatcher.h"
#include "constants.h"
#include "globals.h"
#include "types.h"
#include "sw.h"
#include "get_version.h"
#include "get_app_name.h"
#include "get_public_key.h"
#include "get_app_configuration.h"
#include "sign_tx_stream.h"

int apdu_dispatcher(const command_t *cmd) {
    LEDGER_ASSERT(cmd != NULL, "NULL cmd");

    if (cmd->cla != CLA) {
        return io_send_sw(SWO_INVALID_CLA);
    }

    buffer_t buf = {0};

    switch (cmd->ins) {
        case INS_GET_VERSION:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }
            return handler_get_version();

        case INS_GET_APP_NAME_UTIL:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }
            return handler_get_app_name();

        case INS_GET_APP_CONFIGURATION:
            if (cmd->p1 != 0 || cmd->p2 != 0) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }
            return handler_get_app_configuration();

        case INS_GET_PUBLIC_KEY:
            if (cmd->p1 > 1) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }
            if (!cmd->data) {
                return io_send_sw(SWO_WRONG_DATA_LENGTH);
            }
            buf.ptr = cmd->data;
            buf.size = cmd->lc;
            buf.offset = 0;
            return handler_get_public_key(&buf, (bool) cmd->p1, cmd->p2);

        case INS_SIGN_TX_STREAM:
            if (cmd->p1 != P1_SIGN_INIT && cmd->p1 != P1_SIGN_ADD && cmd->p1 != P1_SIGN_LAST) {
                return io_send_sw(SWO_INCORRECT_P1_P2);
            }
            if (!cmd->data && cmd->lc != 0) {
                return io_send_sw(SWO_WRONG_DATA_LENGTH);
            }
            /* ADD/LAST may use Lc=0 for an empty final chunk. */
            if (cmd->lc == 0) {
                buf.ptr = NULL;
                buf.size = 0;
                buf.offset = 0;
            } else {
                buf.ptr = cmd->data;
                buf.size = cmd->lc;
                buf.offset = 0;
            }
            return handler_sign_tx_stream(&buf, cmd->p1, cmd->p2);

        default:
            return io_send_sw(SWO_INVALID_INS);
    }
}
