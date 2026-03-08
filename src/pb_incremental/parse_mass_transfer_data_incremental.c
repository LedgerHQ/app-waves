#include "pb_parse.h"
#include <stdio.h>
#include <string.h>

bool parse_mass_transfer_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    // В MassTransfer оба интересующих нас поля имеют wire type 2 (length-delimited)
    if (wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                // Тег 1: asset_id (bytes)
                if (tag == waves_MassTransferTransactionData_asset_id_tag) {
                    pb_istream_t s = pb_istream_from_buffer(proto->data + meta, (size_t)body_len);
                    void *arg = (void *)ctx->line1;
                    asset_callback_incremental(&s, NULL, &arg);
                } 
                // Тег 3: attachment (bytes/string)
                else if (tag == waves_MassTransferTransactionData_attachment_tag) {
                    // Используем вашу функцию для текста в line2
                    copy_string_with_dots((char *)ctx->line2, proto->data + meta, (size_t)body_len, 41);
                }
                // Тег 2: transfers (repeated Message) - просто помечаем как обработанное, чтобы сбросить буфер
                else if (tag == 2) {
                    // Мы не декодируем каждый перевод здесь, чтобы не тратить RAM
                    PRINTF("[PB] MassTransfer: skipping individual transfer entry\n");
                }

                return true;
            }
        }
    }

    return false;
}