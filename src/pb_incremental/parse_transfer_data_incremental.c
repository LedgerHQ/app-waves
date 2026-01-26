#include "pb_parse.h"

bool parse_transfer_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, f_size;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    if (wire == 2 && buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &f_size, &s_vlen)) {
        size_t meta = t_vlen + s_vlen;
        uint32_t body_len = (uint32_t)f_size;

        if (proto->data_len >= (meta + body_len)) {
            pb_istream_t substream = pb_istream_from_buffer(proto->data + meta, body_len);

            if (tag == waves_TransferTransactionData_recipient_tag) {
                if (pb_decode_ex(&substream, &waves_Recipient_msg, &ctx->proto.tx.data.transfer.recipient, 1)) {
                    // Конвертация адреса
                    if (ctx->proto.tx.data.transfer.recipient.which_recipient == waves_Recipient_public_key_hash_tag) {
                        waves_public_key_hash_to_address(ctx->proto.tx.data.transfer.recipient.recipient.public_key_hash,
                                                         G_context.signing_context.network_byte,
                                                         G_context.signing_context.ui.line3);
                    } else {
                        strncpy((char *)G_context.signing_context.ui.line3, ctx->proto.tx.data.transfer.recipient.recipient.alias, 31);
                    }
                }
            } 
            else if (tag == waves_TransferTransactionData_amount_tag) {
                ctx->proto.tx.data.transfer.amount.asset_id.funcs.decode = &asset_callback_incremental;
                ctx->proto.tx.data.transfer.amount.asset_id.arg = G_context.signing_context.ui.line2;

                if (pb_decode_ex(&substream, &waves_Amount_msg, &ctx->proto.tx.data.transfer.amount, 1)) {
                    print_amount(ctx->proto.tx.data.transfer.amount.amount, 
                                 G_context.signing_context.amount_decimals,
                                 (unsigned char *) G_context.signing_context.ui.line1, 22);
                }
            } else if (tag == waves_TransferTransactionData_attachment_tag) {
            // Если мы накопили либо все тело, либо хотя бы часть для показа
            size_t available = proto->data_len - meta;
            
            // Ждем либо конца поля, либо когда накопим достаточно для заполнения line4
            if (proto->data_len >= (meta + body_len) || available >= 41) {
                size_t to_copy = (body_len > 41) ? 41 : body_len;
                
                // Очищаем line4 перед копированием
                explicit_bzero(G_context.signing_context.ui.line4, sizeof(G_context.signing_context.ui.line4));
                
                // Копируем данные (они уже в proto->data после meta-данных)
                memmove(G_context.signing_context.ui.line4, proto->data + meta, to_copy);
                
                if (body_len > 41) {
                    // Добавляем многоточие, если текст обрезан
                    memmove(&G_context.signing_context.ui.line4[41], "...", 4);
                } else {
                    G_context.signing_context.ui.line4[to_copy] = '\0';
                }

                PRINTF("[PB] Attachment captured: %s\n", G_context.signing_context.ui.line4);
                return true; // Сбрасываем буфер, так как поле обработано (или его начало)
            }
            return false; // Ждем донакопления
        }
            
            return true; // Поле полностью обработано
        }
    } else if (wire != 2) {
        // Пропускаем varint поля (Timestamp и т.д. внутри трансфера, если они есть)
        return true; 
    }

    return false; // Еще не накопили достаточно данных
}