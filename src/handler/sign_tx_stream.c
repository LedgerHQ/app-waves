/*
 * WAVES Ledger Application — streaming blind sign.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cx.h"
#include "os.h"
#include "buffer.h"
#include "io.h"

#include "sign_tx_stream.h"
#include "constants.h"
#include "globals.h"
#include "sw.h"
#include "display.h"
#include "crypto/path_policy.h"
#include "crypto/secure_hash.h"

/** Drop signing session and clear path, chain_id, hash state, and union. */
static void sign_session_abort(void) {
    explicit_bzero(&G_context, sizeof(G_context));
}

static uint16_t hash_chunk(const uint8_t *data, size_t len) {
    if (len > 0 && data == NULL) {
        return SWO_INCORRECT_DATA;
    }
    uint64_t next = (uint64_t) G_context.sign.received + (uint64_t) len;
    if (next > UINT32_MAX || next > G_context.sign.total_expected) {
        return SWO_INCORRECT_DATA;
    }
    cx_err_t e = waves_secure_hash_update(&G_context.sign.hash_ctx, data, len);
    if (e != CX_OK) {
        return SWO_INCORRECT_DATA;
    }
    G_context.sign.received = (uint32_t) next;
    return SWO_SUCCESS;
}

static bool consume_rest(buffer_t *cdata, uint16_t *err) {
    if (cdata->offset > cdata->size) {
        *err = SWO_INCORRECT_DATA;
        return false;
    }
    size_t rest = cdata->size - cdata->offset;
    *err = hash_chunk(cdata->ptr + cdata->offset, rest);
    if (*err != SWO_SUCCESS) {
        return false;
    }
    cdata->offset += rest;
    return true;
}

int handler_sign_tx_stream(buffer_t *cdata, uint8_t p1, uint8_t chain_id) {
    if (cdata == NULL) {
        sign_session_abort();
        return io_send_sw(SWO_WRONG_DATA_LENGTH);
    }

    if (p1 == P1_SIGN_INIT) {
        explicit_bzero(&G_context, sizeof(G_context));
        G_context.chain_id = chain_id;

        uint8_t levels = 0;
        if (!buffer_read_u8(cdata, &levels) || levels != WAVES_BIP32_DEPTH) {
            sign_session_abort();
            return io_send_sw(SWO_WRONG_DATA_LENGTH);
        }
        if (!buffer_read_bip32_path(cdata, G_context.bip32_path, WAVES_BIP32_DEPTH)) {
            sign_session_abort();
            return io_send_sw(SWO_WRONG_DATA_LENGTH);
        }
        if (!waves_bip32_path_valid(G_context.bip32_path, WAVES_BIP32_DEPTH)) {
            sign_session_abort();
            return io_send_sw(SWO_INCORRECT_DATA);
        }

        uint32_t total_be = 0;
        if (!buffer_read_u32(cdata, &total_be, BE)) {
            sign_session_abort();
            return io_send_sw(SWO_WRONG_DATA_LENGTH);
        }
        if (total_be == 0 || total_be > MAX_TX_STREAM_TOTAL) {
            sign_session_abort();
            return io_send_sw(SWO_INCORRECT_DATA);
        }
        G_context.sign.total_expected = total_be;

        if (waves_secure_hash_init(&G_context.sign.hash_ctx) != CX_OK) {
            sign_session_abort();
            return io_send_sw(SWO_INCORRECT_DATA);
        }

        uint16_t herr = 0;
        if (!consume_rest(cdata, &herr)) {
            sign_session_abort();
            return io_send_sw(herr);
        }

        G_context.sign.session = SIGN_SESSION_STREAMING;
        G_context.bip32_path_len = WAVES_BIP32_DEPTH;
        return io_send_sw(SWO_SUCCESS);
    }

    if (G_context.sign.session != SIGN_SESSION_STREAMING) {
        /* Do not wipe while blind-sign UI holds hash/path for approval. */
        if (G_context.state != STATE_SIGN_STREAM_UI) {
            explicit_bzero(&G_context, sizeof(G_context));
        }
        return io_send_sw(SWO_WAVES_SIGN_NO_INIT);
    }
    if (G_context.chain_id != chain_id) {
        sign_session_abort();
        return io_send_sw(SWO_INCORRECT_P1_P2);
    }

    if (p1 == P1_SIGN_ADD) {
        uint16_t herr = 0;
        if (!consume_rest(cdata, &herr)) {
            sign_session_abort();
            return io_send_sw(herr);
        }
        return io_send_sw(SWO_SUCCESS);
    }

    if (p1 == P1_SIGN_LAST) {
        uint16_t herr = 0;
        if (!consume_rest(cdata, &herr)) {
            sign_session_abort();
            return io_send_sw(herr);
        }

        if (G_context.sign.received != G_context.sign.total_expected) {
            sign_session_abort();
            return io_send_sw(SWO_INCORRECT_DATA);
        }

        if (waves_secure_hash_final(&G_context.sign.hash_ctx, G_context.sign.secure_hash) !=
            CX_OK) {
            sign_session_abort();
            return io_send_sw(SWO_INCORRECT_DATA);
        }

        /* Stop accepting stream chunks; host ADD/LAST must not mutate hash during review. */
        G_context.sign.session = SIGN_SESSION_IDLE;
        G_context.state = STATE_SIGN_STREAM_UI;
        return ui_display_blind_signing();
    }

    sign_session_abort();
    return io_send_sw(SWO_INCORRECT_P1_P2);
}
