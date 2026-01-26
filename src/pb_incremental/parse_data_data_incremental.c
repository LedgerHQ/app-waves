#include "pb_parse.h"

bool parse_data_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    // Читаем тег поля внутри DataTransactionData
    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint8_t wire = (uint8_t)(tag_raw & 0x07);

    // Если это поле типа Length-delimited (строки, байты, вложенные сообщения)
    if (wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                // Мы просто подтверждаем, что поле получено, и ничего не сохраняем
                PRINTF("[PB] DataTx: skipping field, length %u\n", (uint32_t)body_len);
                return true; 
            }
        }
    } 
    // Если это Varint или фиксированные типы
    else {
        uint64_t val;
        size_t v_len;
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &val, &v_len)) {
            return true;
        }
    }

    return false;
}