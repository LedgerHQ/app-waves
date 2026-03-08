#include "pb_parse.h"
#include <stdio.h>
#include <string.h>

bool parse_set_asset_script_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    // Читаем тег поля
    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    // (Asset ID и Script) wire type 2 (Length-delimited)
    if (wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                // asset_id (bytes)
                if (tag == waves_SetAssetScriptTransactionData_asset_id_tag) {
                    pb_istream_t s = pb_istream_from_buffer(proto->data + meta, (size_t)body_len);
                    void *arg = (void *)ctx->line1;
                    
                    asset_callback_incremental(&s, NULL, &arg);
                } 
                else {
                    PRINTF("[PB] SetAssetScript: skipping field %u, len %u\n", tag, (uint32_t)body_len);
                }

                return true;
            }
        }
    }

    return false;
}