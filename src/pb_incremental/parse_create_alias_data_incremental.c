#include "pb_parse.h"

bool parse_create_alias_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    // Читаем тег (Tag 1, Wire 2 для Alias)
    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    // Тег 1: alias (string)
    if (tag == waves_CreateAliasTransactionData_alias_tag && wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                // Очищаем line1 перед записью (если не было префикса)
                explicit_bzero(ctx->line1, sizeof(ctx->line1));

                // Копируем строку напрямую из буфера накопления
                // Используем лимит 41 символ, как в Issue
                copy_string_with_dots(
                    (char *)ctx->line1, 
                    proto->data + meta, 
                    (size_t)body_len, 
                    41
                );

                PRINTF("[PB] Alias captured: %s\n", (char *)ctx->line1);
                return true;
            }
        }
    }

    return false;
}