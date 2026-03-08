#include "pb_parse.h"
#include "pb_decode.h"
#include "print_amount.h"
#include "../globals.h"

// Вспомогательная функция для Varint
static bool peek_varint(const uint8_t *ptr, size_t len, uint64_t *value, size_t *v_len) {
    *value = 0; size_t shift = 0;
    for (size_t i = 0; i < len && i < 10; i++) {
        uint8_t b = ptr[i];
        *value |= (uint64_t)(b & 0x7F) << shift;
        if (!(b & 0x80)) { *v_len = i + 1; return true; }
        shift += 7;
    }
    return false;
}

void pb_process_layer1_field(uiProtobuf_t *ctx, uint32_t tag, uint8_t *data, size_t len) {
    pb_istream_t stream = pb_istream_from_buffer(data, len);

    if (tag == waves_Transaction_sender_public_key_tag) {
        memmove(G_context.signing_context.ui.from, data, 32);
        PRINTF("[PB] Sender OK\n");
    } 
    else if (tag == waves_Transaction_fee_tag) {
        // Мы используем коллбэки из вашего основного кода для Asset ID
        if (pb_decode_ex(&stream, &waves_Amount_msg, &ctx->tx.fee, 1)) {
            print_amount(ctx->tx.fee.amount, G_context.signing_context.fee_decimals, 
                         (unsigned char *)G_context.signing_context.ui.fee_amount, 22);
        }
    }
}

bool pb_incremental_parse(uiProtobuf_t *ctx, buffer_t *chunk, uint32_t total_size) {
    if (!ctx->initialized) {
        explicit_bzero(ctx, sizeof(uiProtobuf_t));
        ctx->total_size = total_size;
        if(cx_blake2b_init_no_throw(&ctx->hash_ctx, 256)) {
            THROW(SW_HASHING_ERROR);
        }
        ctx->initialized = true;
        ctx->state = PB_STATE_ROOT;
    }

    while (buffer_remaining(chunk) > 0 && ctx->total_received < ctx->total_size) {
        uint8_t b = chunk->ptr[chunk->offset];
        if(cx_hash_no_throw((cx_hash_t *)&ctx->hash_ctx, 0, &b, 1, NULL, 0)) {
            THROW(SW_HASHING_ERROR);
        }

        if (ctx->state == PB_STATE_SKIP) {
            ctx->pending_bytes--;
            if (ctx->pending_bytes == 0) ctx->state = PB_STATE_ROOT;
        } 
        else if (ctx->state == PB_STATE_SUBSTREAM) {
            pb_process_layer2_field(ctx, b);
            ctx->pending_bytes--;
            if (ctx->pending_bytes == 0) {
                ctx->state = PB_STATE_ROOT;
                ctx->data_len = 0;
            }
        } 
        else { // PB_STATE_ROOT
            if (ctx->data_len < sizeof(ctx->data)) {
                ctx->data[ctx->data_len++] = b;
            }

            uint64_t tag_raw, f_size;
            size_t t_vlen, s_vlen;

            if (peek_varint(ctx->data, ctx->data_len, &tag_raw, &t_vlen)) {
                uint32_t tag = (uint32_t)(tag_raw >> 3);
                uint8_t wire = (uint8_t)(tag_raw & 0x07);

                if (wire == 2 && peek_varint(ctx->data + t_vlen, ctx->data_len - t_vlen, &f_size, &s_vlen)) {
                    uint32_t body = (uint32_t)f_size;
                    size_t meta = t_vlen + s_vlen;

                    if (tag == 104 || tag == 116) {
                        ctx->state = PB_STATE_SUBSTREAM;
                        ctx->pending_bytes = body;
                        ctx->current_sub_tag = tag;
                        ctx->data_len = 0;
                        PRINTF("[PB] Substream Start Tag %u\n", tag);
                    } else if (tag == 2 || tag == 3) {
                        if (ctx->data_len >= (meta + body)) {
                            pb_process_layer1_field(ctx, tag, ctx->data + meta, body);
                            ctx->data_len = 0;
                        }
                    } else if (meta + body > sizeof(ctx->data)) {
                        ctx->state = PB_STATE_SKIP;
                        ctx->pending_bytes = body - (ctx->data_len - meta);
                        ctx->data_len = 0;
                    }
                } else if (wire != 2 && t_vlen < ctx->data_len) {
                    ctx->data_len = 0; // Skip varints
                }
            }
        }
        chunk->offset++;
        ctx->total_received++;
    }

    if (ctx->total_received >= ctx->total_size) {
        G_context.signing_context.step = 7;
        return true;
    }
    return false;
}