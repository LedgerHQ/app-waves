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
#include "base58.h"
#include "io.h"

#include "display.h"
#include "globals.h"
#include "sw.h"
#include "address.h"
#include "validate.h"
#include "menu.h"

static char g_network_line[40];
static char g_hash_b58[72];
static nbgl_contentTagValue_t pairs[2];
static nbgl_contentTagValueList_t pairList;

static void blind_review_choice(bool confirm) {
    validate_blind_sign(confirm);
    if (confirm) {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_SIGNED, ui_menu_main);
    } else {
        nbgl_useCaseReviewStatus(STATUS_TYPE_TRANSACTION_REJECTED, ui_menu_main);
    }
}

int ui_display_blind_signing(void) {
    if (G_context.state != STATE_SIGN_STREAM_UI) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_CONDITIONS_NOT_SATISFIED);
    }

    explicit_bzero(g_network_line, sizeof(g_network_line));
    explicit_bzero(g_hash_b58, sizeof(g_hash_b58));
    waves_format_network_line(G_context.chain_id, g_network_line, sizeof(g_network_line));

    if (base58_encode(G_context.sign.secure_hash,
                      sizeof(G_context.sign.secure_hash),
                      g_hash_b58,
                      sizeof(g_hash_b58)) < 0) {
        explicit_bzero(&G_context, sizeof(G_context));
        return io_send_sw(SWO_INCORRECT_DATA);
    }

    pairs[0].item = "Network";
    pairs[0].value = g_network_line;
    pairs[1].item = "Tx hash";
    pairs[1].value = g_hash_b58;

    pairList.nbMaxLinesForValue = 0;
    pairList.nbPairs = 2;
    pairList.pairs = pairs;
    pairList.wrapping = true;

    nbgl_useCaseReviewBlindSigning(TYPE_TRANSACTION,
                                   &pairList,
                                   &ICON_APP_WAVES,
                                   "Blind signing",
                                   NULL,
                                   "Sign transaction hash?",
                                   NULL,
                                   blind_review_choice);
    return 0;
}
