/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <string.h>

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"
#include "io.h"

#include "display.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"
#include "address.h"
#include "validate.h"
#include "menu.h"

static char g_address[WAVES_ADDRESS_STR_MAX];

static void review_choice(bool confirm) {
    validate_pubkey(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_ADDRESS_VERIFIED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_ADDRESS_REJECTED, ui_menu_main);
    }
}

int ui_display_address(void) {
    if (G_context.state != STATE_CONFIRM_ADDRESS || G_context.req_display_address != 1) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_CONDITIONS_NOT_SATISFIED);
    }

    explicit_bzero(g_address, sizeof(g_address));
    if (!waves_encode_address(G_context.chain_id,
                              G_context.pk_info.public_key,
                              g_address,
                              sizeof(g_address))) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_INCORRECT_DATA);
    }

    nbgl_useCaseAddressReview(g_address,
                              NULL,
                              &ICON_APP_WAVES,
                              "Verify Waves address",
                              NULL,
                              review_choice);
    return 0;
}
