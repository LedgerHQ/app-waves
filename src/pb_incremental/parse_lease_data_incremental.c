#include "pb_parse.h"

bool parse_lease_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);
    
    if (tag == waves_LeaseTransactionData_recipient_tag && wire == 2) {
        if (!buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) return false;
        if (proto->data_len < (t_vlen + s_vlen + body_len)) return false;
        pb_istream_t s = pb_istream_from_buffer(proto->data + t_vlen + s_vlen, (size_t)body_len);
        waves_Recipient rec = waves_Recipient_init_zero;
        if (pb_decode(&s, waves_Recipient_fields, &rec)) {
            if (rec.which_recipient == waves_Recipient_public_key_hash_tag) {
                waves_public_key_hash_to_address(rec.recipient.public_key_hash, G_context.signing_context.network_byte, ctx->line3);
            } else {
                snprintf((char*)ctx->line3, sizeof(ctx->line3), "%s", rec.recipient.alias);
            }
        }
        return true;
    } else if (tag == waves_LeaseTransactionData_amount_tag && wire == 0) {
        uint64_t val; size_t v_len;
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &val, &v_len)) {
            print_amount(val, G_context.signing_context.amount_decimals, (unsigned char*)ctx->line1, 22);
            return true;
        }
    }
    return false;
}