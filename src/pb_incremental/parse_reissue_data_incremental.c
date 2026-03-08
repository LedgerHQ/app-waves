#include "pb_parse.h"

bool parse_reissue_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    // Пытаемся прочитать тег поля внутри ReissueData
    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    // Поля Reissue: 1: asset_amount (Amount), 2: reissuable (bool)
    if (tag == waves_ReissueTransactionData_asset_amount_tag && wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                // Декодируем вложенный waves_Amount (AssetID + Amount)
                pb_istream_t stream = pb_istream_from_buffer(proto->data + meta, (size_t)body_len);
                waves_Amount am = waves_Amount_init_zero;
                
                // Настраиваем колбэк для Asset ID (линия 2)
                am.asset_id.funcs.decode = &asset_callback_incremental;
                am.asset_id.arg = ctx->line2;

                if (pb_decode(&stream, waves_Amount_fields, &am)) {
                    // Форматируем сумму в линию 1
                    print_amount(am.amount, 
                                 G_context.signing_context.amount_decimals,
                                 (unsigned char *)ctx->line1, 
                                 sizeof(ctx->line1));
                }
                return true; // Поле обработано
            }
        }
    } 
else if (tag == waves_ReissueTransactionData_reissuable_tag && wire == 0) {
        uint64_t is_reissuable;
        size_t v_len;
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &is_reissuable, &v_len)) {
            if (is_reissuable) {
                snprintf((char *)ctx->line3, sizeof(ctx->line3), "True");
            } else {
                snprintf((char *)ctx->line3, sizeof(ctx->line3), "False");
            }
            return true;
        }
    }

    return false;
}