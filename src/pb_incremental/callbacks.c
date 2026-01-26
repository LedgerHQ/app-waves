#include "bs58.h"
#include "pb_parse.h"

int get_waves_transaction_header(uint32_t tag) {
    switch (tag) {
        case waves_Transaction_sender_public_key_tag:
            return 1;
        case waves_Transaction_fee_tag:
            return 2;
        default:
            return 0;  
    }
}


int get_waves_transaction_type(uint32_t tag) {
    switch (tag) {
        case waves_Transaction_issue_tag:              return 3;   // 103
        case waves_Transaction_transfer_tag:           return 4;   // 104
        case waves_Transaction_reissue_tag:            return 5;   // 105
        case waves_Transaction_burn_tag:               return 6;   // 106
        case waves_Transaction_lease_tag:              return 8;   // 108
        case waves_Transaction_lease_cancel_tag:       return 9;   // 109
        case waves_Transaction_create_alias_tag:       return 10;  // 110
        case waves_Transaction_mass_transfer_tag:      return 11;  // 111
        case waves_Transaction_data_transaction_tag:   return 12;  // 112
        case waves_Transaction_set_script_tag:         return 13;  // 113
        case waves_Transaction_sponsor_fee_tag:        return 14;  // 114
        case waves_Transaction_set_asset_script_tag:   return 15;  // 115
        case waves_Transaction_invoke_script_tag:      return 16;  // 116
        case waves_Transaction_update_asset_info_tag:  return 17;  // 117
        default:
            return 0;
    }
}

void copy_string_with_dots(char *dest, uint8_t *src, size_t src_len, size_t max_chars) {
    // Вычисляем, сколько реально байт копируем (не больше лимита)
    size_t to_copy = (src_len > max_chars) ? max_chars : src_len;
    
    // Очищаем буфер назначения (max_chars + 4 для "...\0")
    explicit_bzero(dest, max_chars + 4);
    
    // Копируем данные
    memmove(dest, src, to_copy);
    
    if (src_len > max_chars) {
        // Добавляем многоточие, если оригинал длиннее лимита
        memmove(&dest[max_chars], "...", 4);
    } else {
        // Убеждаемся, что строка закрыта нулем
        dest[to_copy] = '\0';
    }
}

bool asset_callback_incremental(pb_istream_t *stream, const pb_field_iter_t *field, void **arg) {
    UNUSED(field);

    // Буфер для хранения сырых 32 байт ID ассета
    uint8_t temp_id[32];
    // Максимальная длина Base58 строки для 32 байт + null-terminator (обычно 44-45 символов)
    size_t b58_len = 45;

    // stream->bytes_left показывает размер поля bytes в Protobuf
    size_t len = stream->bytes_left;

    if (len == 32) {
        // Читаем 32 байта из потока
        if (!pb_read(stream, temp_id, 32)) {
            return false;
        }

        // Кодируем в Base58 напрямую в буфер, переданный через arg
        if (!b58enc((char *) *arg, &b58_len, (const void *) temp_id, 32)) {
            return false;
        }

        PRINTF("[PB] Asset ID encoded: %s\n", (char *) *arg);
    } else if (len == 0) {
        // Если длина 0, это нативный токен WAVES
        // Копируем константу "WAVES" в буфер
        memmove((char *) *arg, WAVES_CONST, sizeof(WAVES_CONST));
        PRINTF("[PB] Asset: WAVES\n");
    } else {
        // Непредвиденная длина ассета (в Waves это всегда либо 0, либо 32)
        PRINTF("[PB] Error: Invalid Asset ID length: %u\n", len);
        // Пропускаем "битые" данные, чтобы не сломать парсинг всей транзакции
        return pb_read(stream, NULL, len);
    }

    return true;
}
