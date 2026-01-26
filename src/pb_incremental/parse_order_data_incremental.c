#include "pb_parse.h"
#include "pb_decode.h"
#include "print_amount.h"
#include "../globals.h"
#include <string.h>

static bool peek_varint(const uint8_t *ptr, size_t len, uint64_t *value, size_t *v_len) {
    *value = 0; 
    size_t shift = 0;
    for (size_t i = 0; i < len && i < 10; i++) {
        uint8_t b = ptr[i];
        *value |= (uint64_t)(b & 0x7F) << shift;
        if (!(b & 0x80)) { 
            *v_len = i + 1; 
            return true; 
        }
        shift += 7;
    }
    return false;
}

void pb_process_order_fields(uiProtobuf_t *ctx, uint8_t b) {
    if (ctx->data_len < sizeof(ctx->data)) {
        ctx->data[ctx->data_len++] = b;
    }

    uint64_t tag_raw, f_size;
    size_t t_vlen, s_vlen;

    if (peek_varint(ctx->data, ctx->data_len, &tag_raw, &t_vlen)) {
        uint32_t tag = (uint32_t)(tag_raw >> 3);
        uint8_t wire = (uint8_t)(tag_raw & 0x07);

        // --- WIRE 2: Messages & Bytes ---
        if (wire == 2 && peek_varint(ctx->data + t_vlen, ctx->data_len - t_vlen, &f_size, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            uint32_t body = (uint32_t)f_size;

            if (ctx->data_len >= (meta + body)) {
                pb_istream_t stream = pb_istream_from_buffer(ctx->data + meta, body);

                switch (tag) {
                    case waves_Order_matcher_public_key_tag: // Tag 3
                        memmove(G_context.signing_context.ui.line2, ctx->data + meta, 32);
                        waves_public_key_to_address((uint8_t*)G_context.signing_context.ui.line2,
                                                    G_context.signing_context.network_byte,
                                                    (unsigned char *)G_context.signing_context.ui.line2);
                        PRINTF("[UI] Matcher: %s\n", G_context.signing_context.ui.line2);
                        break;

                    case waves_Order_asset_pair_tag: // Tag 4
                        {
                            waves_AssetPair pair = waves_AssetPair_init_zero;
                            pair.amount_asset_id.funcs.decode = &asset_callback_incremental;
                            pair.amount_asset_id.arg = G_context.signing_context.ui.line1;
                            pb_decode(&stream, waves_AssetPair_fields, &pair);
                        }
                        break;

                    case waves_Order_matcher_fee_tag: // Tag 10 (В твоем файле это Message)
                        {
                            waves_Amount fee = waves_Amount_init_zero;
                            fee.asset_id.funcs.decode = &asset_callback_incremental;
                            fee.asset_id.arg = G_context.signing_context.ui.fee_asset;
                            if (pb_decode(&stream, waves_Amount_fields, &fee)) {
                                print_amount(fee.amount, G_context.signing_context.fee_decimals, 
                                             (unsigned char *)G_context.signing_context.ui.fee_amount, 22);
                            }
                        }
                        break;
                }
                ctx->data_len = 0;
            }
        } 
        // --- WIRE 0: Varints ---
        else if (wire == 0) {
            uint64_t val;
            if (peek_varint(ctx->data + t_vlen, ctx->data_len - t_vlen, &val, &s_vlen)) {
                switch (tag) {
                    case waves_Order_order_side_tag: // Tag 5
                        strncpy((char *)G_context.signing_context.ui.line3, (val == 0) ? "Buy order" : "Sell order", 15);
                        break;
                    case waves_Order_amount_tag: // Tag 6
                        print_amount(val, G_context.signing_context.amount_decimals, 
                                     (unsigned char *)G_context.signing_context.ui.line4, 22);
                        break;
                    case waves_Order_price_tag: // Tag 7
                        print_amount(val, 8, (unsigned char *)G_context.signing_context.ui.line5, 22);
                        break;
                    // Tag 8 (Timestamp) и 9 (Expiration) просто пропускаем, они не нужны для UI
                }
                ctx->data_len = 0;
            }
        }
    }
}

bool pb_incremental_parse_order(uiContext_t *ui_ctx, buffer_t *chunk, uint32_t total_size) {
    uiProtobuf_t *ctx = &ui_ctx->proto;
    if (!ctx->initialized) {
        explicit_bzero(ctx, sizeof(uiProtobuf_t));
        ctx->total_size = total_size;
        if(cx_blake2b_init_no_throw(&ctx->hash_ctx, 256)) {
            THROW(SW_HASHING_ERROR);
        }
        ctx->initialized = true;
        ctx->state = PB_STATE_SUBSTREAM;
    }

    while (buffer_remaining(chunk) > 0 && ctx->total_received < ctx->total_size) {
        uint8_t b = chunk->ptr[chunk->offset];
        if(cx_hash_no_throw((cx_hash_t *)&ctx->hash_ctx, 0, &b, 1, NULL, 0)) {
            THROW(SW_HASHING_ERROR);
        }
        pb_process_order_fields(ctx, b); 
        chunk->offset++;
        ctx->total_received++;
    }

    if (ctx->total_received >= ctx->total_size) {
        uint8_t computed_hash[32];
        if(cx_hash_no_throw((cx_hash_t *)&ctx->hash_ctx, CX_LAST, NULL, 0, computed_hash, 32)) {
            THROW(SW_HASHING_ERROR);
        }
        
        if (memcmp(computed_hash, G_context.signing_context.first_data_hash, 32) != 0) {
            THROW(SW_HASHING_ERROR); 
        }

        if (strlen((char *)G_context.signing_context.ui.line1) == 0) {
            strncpy((char *)G_context.signing_context.ui.line1, "WAVES", 6);
            G_context.signing_context.amount_decimals = 8;
        }
        if (strlen((char *)G_context.signing_context.ui.fee_asset) == 0) {
            strncpy((char *)G_context.signing_context.ui.fee_asset, "WAVES", 6);
            G_context.signing_context.fee_decimals = 8;
        }

        PRINTF("[OK] Order ready for display\n");
        G_context.signing_context.step = 7;
        return true;
    }
    return false;
}