#include "pb_parse.h"

bool parse_burn_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    // Read Tag BurnTransactionData
    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    if (tag == waves_BurnTransactionData_asset_amount_tag && wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                pb_istream_t stream = pb_istream_from_buffer(proto->data + meta, (size_t)body_len);
                waves_Amount am = waves_Amount_init_zero;
                
                am.asset_id.funcs.decode = &asset_callback_incremental;
                am.asset_id.arg = ctx->line2;

                if (pb_decode(&stream, waves_Amount_fields, &am)) {
                    size_t prefix_len = strlen((char *)ctx->line1);
                    print_amount(am.amount, 
                                 G_context.signing_context.amount_decimals, 
                                 (unsigned char *)ctx->line1 + prefix_len, 
                                 sizeof(ctx->line1) - prefix_len);
                }
                return true;
            }
        }
    }

    return false;
}