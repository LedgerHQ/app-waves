/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdint.h>
#include <string.h>

#include "os.h"
#include "ux.h"

#include "types.h"
#include "globals.h"
#include "io.h"
#include "sw.h"
#include "menu.h"
#include "dispatcher.h"

global_ctx_t G_context;

void app_main(void) {
    int input_len = 0;
    command_t cmd;

    io_init();
    ui_menu_main();

    explicit_bzero(&G_context, sizeof(G_context));

    for (;;) {
        if ((input_len = io_recv_command()) < 0) {
            PRINTF("=> io_recv_command failure\n");
            return;
        }

        if (!apdu_parser(&cmd, G_io_apdu_buffer, (uint16_t) input_len)) {
#if defined(DEBUG)
            PRINTF("=> /!\\ BAD LENGTH: %.*H\n", input_len, G_io_apdu_buffer);
#else
            PRINTF("=> /!\\ BAD LENGTH\n");
#endif
            io_send_sw(SWO_WRONG_DATA_LENGTH);
            continue;
        }

#if defined(DEBUG)
        PRINTF("=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | CData=%.*H\n",
               cmd.cla,
               cmd.ins,
               cmd.p1,
               cmd.p2,
               cmd.lc,
               cmd.lc,
               cmd.data);
#else
        PRINTF("=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X\n",
               cmd.cla,
               cmd.ins,
               cmd.p1,
               cmd.p2,
               cmd.lc);
#endif

        if (apdu_dispatcher(&cmd) < 0) {
            PRINTF("=> apdu_dispatcher failure\n");
            return;
        }
    }
}
