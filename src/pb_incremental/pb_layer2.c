#include "pb_parse.h"
#include "pb_decode.h"
#include "print_amount.h"
#include "../globals.h"

// Вспомогательная для под-потока (аналогична Layer 1)
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

void pb_process_layer2_field(uiProtobuf_t *ctx, uint8_t b) {
    // Накапливаем байты под-потока для поиска тегов ВНУТРИ TransferData
    if (ctx->data_len < sizeof(ctx->data)) {
        ctx->data[ctx->data_len++] = b;
    }

    uint64_t tag_raw, f_size;
    size_t t_vlen, s_vlen;

    if (peek_varint(ctx->data, ctx->data_len, &tag_raw, &t_vlen)) {
        uint32_t tag = (uint32_t)(tag_raw >> 3);
        uint8_t wire = (uint8_t)(tag_raw & 0x07);

        if (wire == 2 && peek_varint(ctx->data + t_vlen, ctx->data_len - t_vlen, &f_size, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            uint32_t body = (uint32_t)f_size;

            if (ctx->data_len >= (meta + body)) {
                pb_istream_t stream = pb_istream_from_buffer(ctx->data + meta, body);
                
                if (ctx->current_sub_tag == 104) { // Transfer
                    if (tag == waves_TransferTransactionData_recipient_tag) {
                        pb_decode_ex(&stream, &waves_Recipient_msg, &ctx->tx.data.transfer.recipient, 1);
                        // Тут будет логика конвертации адреса в line3
                        PRINTF("[PB] Sub-Recipient Found\n");
                    } else if (tag == waves_TransferTransactionData_amount_tag) {
                        pb_decode_ex(&stream, &waves_Amount_msg, &ctx->tx.data.transfer.amount, 1);
                        PRINTF("[PB] Sub-Amount Found\n");
                    }
                }
                ctx->data_len = 0; // Сброс для следующего тега внутри суб-потока
            }
        } else if (wire != 2 && t_vlen < ctx->data_len) {
            ctx->data_len = 0;
        }
    }
}