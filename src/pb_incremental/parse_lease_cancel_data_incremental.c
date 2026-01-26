#include "pb_parse.h"

bool parse_lease_cancel_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t) (tag_raw >> 3);
    // uint8_t wire = (uint8_t) (tag_raw & 0x07);

    if (tag == waves_LeaseCancelTransactionData_lease_id_tag) {
        if (buffer_peek_varint(proto->data + t_vlen,
                               proto->data_len - t_vlen,
                               &body_len,
                               &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t) body_len)) {
                // 1. tmp stream
                pb_istream_t temp_stream =
                    pb_istream_from_buffer(proto->data + meta, (size_t) body_len);

                void *arg = (void *) ctx->line1;

                asset_callback_incremental(&temp_stream, NULL, &arg);

                return true;
            }
        }
    }
    return false;
}