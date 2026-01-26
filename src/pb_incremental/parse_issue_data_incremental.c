#include "pb_parse.h"

bool parse_issue_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;
    uint64_t tag_raw, body_len;
    size_t t_vlen, s_vlen;

    if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_vlen)) {
        return false;
    }

    uint32_t tag = (uint32_t)(tag_raw >> 3);
    uint8_t wire = (uint8_t)(tag_raw & 0x07);

  
// line 1 - name
// line 2 - description
// line 3 - amount
// line 5 - reissuable
// line 6 - has script

    if (wire == 2) {
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &body_len, &s_vlen)) {
            size_t meta = t_vlen + s_vlen;
            if (proto->data_len >= (meta + (size_t)body_len)) {
                
                if (tag == waves_IssueTransactionData_name_tag) {
                    size_t prefix_len = strlen((char *)ctx->line1);
                    copy_string_with_dots((char *)ctx->line1 + prefix_len, proto->data + meta, (size_t)body_len, 41 - prefix_len);
                } 
                else if (tag == waves_IssueTransactionData_description_tag) {
                    copy_string_with_dots((char *)ctx->line2, proto->data + meta, (size_t)body_len, 41);
                }
                else if (tag == waves_IssueTransactionData_script_tag) {
                    // Если поле скрипта присутствует и не пустое
                    if (body_len > 0) {
                        snprintf((char *)ctx->line6, sizeof((char *)ctx->line6), "True");
                    }
                }
                return true;
            }
        }
    } 
    // 2.  Varint (Amount, Decimals, Reissuable) - wire type 0
    else if (wire == 0) {
        uint64_t val;
        size_t v_len;
        if (buffer_peek_varint(proto->data + t_vlen, proto->data_len - t_vlen, &val, &v_len)) {
            
            if (tag == waves_IssueTransactionData_amount_tag) {
                proto->tx.data.issue.amount = val; 
                print_amount(proto->tx.data.issue.amount, (uint8_t)proto->tx.data.issue.decimals, (unsigned char *)ctx->line3, sizeof(ctx->line3));
            }
            else if (tag == waves_IssueTransactionData_decimals_tag) {
                proto->tx.data.issue.decimals = (uint32_t)val;
                snprintf((char *)ctx->line4, sizeof((char *)ctx->line4), "%u", (uint32_t)proto->tx.data.issue.decimals);
                print_amount(proto->tx.data.issue.amount, (uint8_t)proto->tx.data.issue.decimals, (unsigned char *)ctx->line3, sizeof(ctx->line3));
            }
            else if (tag == waves_IssueTransactionData_reissuable_tag) {
                snprintf((char *)ctx->line5, sizeof((char *)ctx->line5), val ? "True" : "False");
            }
            return true;
        }
    }

    return false;
}