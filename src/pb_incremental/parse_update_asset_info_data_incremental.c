#include "pb_parse.h"
#include <stdio.h>
#include <string.h>

bool parse_update_asset_info_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    if (wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                // asset_id (bytes)
                if (tag == waves_UpdateAssetInfoTransactionData_asset_id_tag) {
                    pb_istream_t s = pb_istream_from_buffer(proto->data + meta, (size_t)body_len);
                    void *arg = (void *)ctx->line1;
                    asset_callback_incremental(&s, NULL, &arg);
                } 
                // name (string)
                else if (tag == waves_UpdateAssetInfoTransactionData_name_tag) {
                    copy_string_with_dots((char *)ctx->line2, proto->data + meta, (size_t)body_len, 41);
                }
                // description (string)
                else if (tag == waves_UpdateAssetInfoTransactionData_description_tag) {
                    copy_string_with_dots((char *)ctx->line4, proto->data + meta, (size_t)body_len, 41);
                }

                return true; 
            }
        }
    }

    return false;
}