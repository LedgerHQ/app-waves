#include "pb_parse.h"
#include "pb_decode.h"
#include "print_amount.h"
#include "buffer_helper.h" // Подключаем ваши утилиты

void process_protobuf_field(uiContext_t *ctx, uint32_t tag, uint8_t *data, size_t len) {
    pb_istream_t stream = pb_istream_from_buffer(data, len);

    if (tag == waves_Transaction_sender_public_key_tag) {
        if (len == 32) {
            memmove(G_context.signing_context.ui.from, data, 32);
            PRINTF("[PB] Root: Sender Public Key captured\n");
        }
    }
    else if (tag == waves_Transaction_fee_tag) {
        ctx->proto.tx.fee.asset_id.funcs.decode = &asset_callback_incremental;
        ctx->proto.tx.fee.asset_id.arg = G_context.signing_context.ui.fee_asset;

        if (pb_decode_ex(&stream, &waves_Amount_msg, &ctx->proto.tx.fee, 1)) {
            print_amount(ctx->proto.tx.fee.amount,
                         G_context.signing_context.fee_decimals,
                         (unsigned char *) G_context.signing_context.ui.fee_amount,
                         sizeof(G_context.signing_context.ui.fee_amount));
            PRINTF("[PB] Root: Fee Amount captured\n");
        }
    }
}

bool buffer_peek_varint(const uint8_t *ptr, size_t len, uint64_t *value, size_t *v_len) {
    *value = 0;
    size_t shift = 0;
    for (size_t i = 0; i < len; i++) {
        uint8_t b = ptr[i];
        *value |= (uint64_t) (b & 0x7F) << shift;
        if (!(b & 0x80)) {
            *v_len = i + 1;
            return true;
        }
        shift += 7;
        if (shift >= 64) break;
    }
    return false;
}

void process_layer2_field(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    if (proto->data_len < 2) return;

    bool processed = false;
    uint8_t current_tx_type = G_context.signing_context.data_type;

    switch (proto->current_data_tag) {
        case waves_Transaction_transfer_tag:
            if (current_tx_type != 4) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_transfer_data_incremental(ctx);
            break;

        case waves_Transaction_issue_tag:
            if (current_tx_type != 3) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_issue_data_incremental(ctx);
            break;

        case waves_Transaction_reissue_tag:
            if (current_tx_type != 5) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_reissue_data_incremental(ctx);
            break;

        case waves_Transaction_burn_tag:
            if (current_tx_type != 6) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_burn_data_incremental(ctx);
            break;

        case waves_Transaction_lease_tag:
            if (current_tx_type != 8) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_lease_data_incremental(ctx);
            break;

        case waves_Transaction_lease_cancel_tag:
            if (current_tx_type != 9) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_lease_cancel_data_incremental(ctx);
            break;

        case waves_Transaction_create_alias_tag:
            if (current_tx_type != 10) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_create_alias_data_incremental(ctx);
            break;

        case waves_Transaction_mass_transfer_tag:
            if (current_tx_type != 11) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_mass_transfer_data_incremental(ctx);
            break;

        case waves_Transaction_data_transaction_tag:
            if (current_tx_type != 12) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_data_data_incremental(ctx);
            break;

        case waves_Transaction_set_script_tag:
            if (current_tx_type != 13) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_data_data_incremental(ctx);
            break;

        case waves_Transaction_sponsor_fee_tag:
            if (current_tx_type != 14) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_sponsor_fee_data_incremental(ctx);
            break;

        case waves_Transaction_set_asset_script_tag:
            if (current_tx_type != 15) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_set_asset_script_data_incremental(ctx);
            break;

        case waves_Transaction_invoke_script_tag:
            if (current_tx_type != 16) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_invoke_script_data_incremental(ctx);
            break;

        case waves_Transaction_update_asset_info_tag:
            if (current_tx_type != 17) THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
            processed = parse_update_asset_info_data_incremental(ctx);
            break;

        default:
            // Если пришел тег, который мы вообще не знаем
            THROW(SW_INCORRECT_TRANSACTION_TYPE_VERSION);
    }

    if (processed) {
        proto->data_len = 0;
    }
}

bool build_protobuf_root_tx_incremental(uiContext_t *ctx, buffer_t *chunk, uint32_t total_size) {
    uiProtobuf_t *proto = &ctx->proto;

    if (!proto->initialized) {
        explicit_bzero(proto, sizeof(uiProtobuf_t));
        proto->total_size = total_size;

        if (cx_blake2b_init_no_throw(&proto->hash_ctx, 256) != CX_OK) {
            return false;
        }

        proto->tx = (waves_Transaction) waves_Transaction_init_zero;
        proto->initialized = true;
        PRINTF("[PB] Incremental Decoder Started. Total Size: %u\n", total_size);
    }

    while (buffer_remaining(chunk) > 0 && proto->total_received < proto->total_size) {
        uint8_t b;
        // Читаем байт через вашу утилиту
        if (!buffer_read_next(chunk, &b, 1)) break;

        if(cx_hash_no_throw((cx_hash_t *) &proto->hash_ctx, 0, &b, 1, NULL, 0)) {
            THROW(SW_HASHING_ERROR);
        }
        proto->total_received++;

        // --- СОСТОЯНИЕ 1: ПАРСИНГ ВЛОЖЕННОГО СООБЩЕНИЯ (Layer 2) ---
        if (proto->sub_remaining > 0) {
            if (proto->data_len < sizeof(proto->data)) {
                proto->data[proto->data_len++] = b;
            }

            process_layer2_field(ctx);
            proto->sub_remaining--;

            if (proto->sub_remaining == 0) {
                proto->data_len = 0;
                PRINTF("[PB] Sub-stream Finished\n");
            }
            continue;
        }

        // --- СОСТОЯНИЕ 2: ПРОПУСК ДАННЫХ ---
        if (proto->skip_remaining > 0) {
            proto->skip_remaining--;
            continue;
        }

        // --- СОСТОЯНИЕ 3: ПАРСИНГ КОРНЯ (Layer 1) ---
        if (proto->data_len < sizeof(proto->data)) {
            proto->data[proto->data_len++] = b;
        }

        uint64_t tag_raw, f_size;
        size_t t_vlen, s_vlen;

        if (buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
            uint32_t tag = (uint32_t) (tag_raw >> 3);
            uint8_t wire = (uint8_t) (tag_raw & 0x07);

            if (wire == 2) { // Length-delimited
                if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &f_size, &s_vlen)) {
                    size_t meta = t_vlen + s_vlen;
                    uint32_t body = (uint32_t) f_size;

                    // Проверка на тип транзакции (Issue/Transfer...)
                    if (get_waves_transaction_type(tag)) {
                        proto->current_data_tag = tag;
                        proto->sub_remaining = body;
                        proto->data_len = 0; 
                        PRINTF("[PB] Entering Sub-stream: Tag %u, Size %u\n", tag, body);
                    }
                    // Проверка на заголовки (SenderPK/Fee)
                    else if (get_waves_transaction_header(tag)) {
                        if (proto->data_len >= (meta + body)) {
                            process_protobuf_field(ctx, tag, proto->data + meta, body);
                            proto->data_len = 0;
                        }
                    }
                    // Если поле слишком большое для буфера - в режим пропуска
                    else if (meta + body > sizeof(proto->data)) {
                        proto->skip_remaining = body - (proto->data_len - meta);
                        proto->data_len = 0;
                        PRINTF("[PB] Skipping Field: Tag %u, Left: %u\n", tag, proto->skip_remaining);
                    }
                }
            }
            else if (wire == 0) { // Varint (Chain ID)
                uint64_t val;
                size_t v_len;
                if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &val, &v_len)) {
                    proto->data_len = 0; // Очищаем после успешного прочтения
                }
            }
        }
    }

    // Финализация
    if (proto->total_received >= proto->total_size) {
        if (strlen((char *)G_context.signing_context.ui.fee_asset) == 0) {
            memmove(G_context.signing_context.ui.fee_asset, "WAVES", 6);
        }

        uint8_t computed_hash[32];
        if(cx_hash_no_throw((cx_hash_t *) &proto->hash_ctx, CX_LAST, NULL, 0, computed_hash, 32)) {
            THROW(SW_HASHING_ERROR);
        }
        
        if (memcmp(computed_hash, G_context.signing_context.first_data_hash, 32) != 0) {
            THROW(SW_SIGN_DATA_NOT_MATCH);
        }
        
        G_context.signing_context.step = 7;
        return true;
    }

    return false;
}