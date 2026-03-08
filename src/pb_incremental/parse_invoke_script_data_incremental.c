#include "pb_parse.h"
#include "pb_decode.h"
#include <string.h>

/**
 * Consumes n bytes from the buffer and updates internal pointers/counters.
 * Essential for incremental parsing to prevent re-processing the same data.
 */
void consume_pb_bytes(uiProtobuf_t *proto, size_t n) {
    if (n == 0) return;
    if (n > proto->data_len) n = proto->data_len;

    // Shift remaining data to the front of the buffer
    memmove(proto->data, proto->data + n, proto->data_len - n);
    proto->data_len -= (uint16_t)n;
    proto->total_read += n;
    
    // If we are currently inside a FunctionCall block, decrement the remaining field budget
    if (proto->waiting_for_func_name) {
        proto->invoke_skip_remaining = (proto->invoke_skip_remaining > (uint32_t)n) ? 
                                        proto->invoke_skip_remaining - (uint32_t)n : 0;
    }
}

/**
 * Level 3 Parser: Extracts the function name from a raw binary blob (Tag 2).
 * Since Waves serializes the call as a delimited string, we capture until '|'.
 */
static bool parse_function_call_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;

    if (proto->invoke_skip_remaining > 0 && proto->data_len > 0) {
        // Determine how many bytes we can process in the current chunk
        uint32_t can_take = (proto->data_len < (uint16_t)proto->invoke_skip_remaining) 
                            ? (uint32_t)proto->data_len : proto->invoke_skip_remaining;

        for (uint32_t i = 0; i < can_take; i++) {
            uint8_t b = proto->data[i];

            // 1. Skip Protobuf control bytes (Tags/Length) at the start of the field.
            // ASCII printable characters start from 0x20.
            if (proto->name_bytes_collected == 0 && b < 0x20) {
                continue; 
            }

            // 2. Stop capturing if we hit the delimiter '|' or a null terminator.
            if (b == '|' || b == '\0') {
                if (proto->name_bytes_collected > 0 && proto->name_bytes_collected < 40) {
                    G_context.signing_context.ui.line2[proto->name_bytes_collected] = '\0';
                    // Set flag to 40 to indicate we are done capturing name for this field
                    proto->name_bytes_collected = 40; 
                }
                continue;
            }

            // 3. Copy character to the UI buffer if there is space
            if (proto->name_bytes_collected < 39) {
                G_context.signing_context.ui.line2[proto->name_bytes_collected++] = (char)b;
            }
        }

        // Remove processed bytes from the global protobuf buffer
        consume_pb_bytes(proto, can_take);
    }

    // Reset state once the entire FunctionCall field has been consumed
    if (proto->invoke_skip_remaining == 0) {
        proto->waiting_for_func_name = false;
        if (proto->name_bytes_collected < 40) {
            G_context.signing_context.ui.line2[proto->name_bytes_collected] = '\0';
        }
        PRINTF("[PB] Captured Function Name: %s\n", G_context.signing_context.ui.line2);
    }
    
    return true;
}

/**
 * Level 2 Parser: Main InvokeScript loop.
 * Iterates through top-level Protobuf tags: dApp, FunctionCall, and Payments.
 */
bool parse_invoke_script_data_incremental(uiContext_t *ctx) {
    uiProtobuf_t *proto = &ctx->proto;

    while (proto->data_len > 0) {
        // Redirect data to the function name parser if in 'waiting' mode
        if (proto->waiting_for_func_name) {
            if (!parse_function_call_incremental(ctx)) return false;
            continue;
        }

        uint64_t tag_raw, b_len;
        size_t t_v, l_v;

        // Extract the next Tag and Length (Varints)
        if (!buffer_peek_varint(proto->data, proto->data_len, &tag_raw, &t_v)) return false;
        uint32_t tag = (uint32_t)(tag_raw >> 3);

        if (!buffer_peek_varint(proto->data + t_v, proto->data_len - t_v, &b_len, &l_v)) return false;
        uint32_t field_total_size = t_v + l_v + (uint32_t)b_len;

        // TAG 1: dApp (Recipient)
        if (tag == 1) { 
            if (proto->data_len < field_total_size) return false; 
            waves_Recipient d_app_rec = waves_Recipient_init_zero;
            pb_istream_t s = pb_istream_from_buffer(proto->data + t_v + l_v, (size_t)b_len);
            
            if (pb_decode(&s, waves_Recipient_fields, &d_app_rec)) {
                if (d_app_rec.which_recipient == waves_Recipient_public_key_hash_tag) {
                    waves_public_key_hash_to_address(d_app_rec.recipient.public_key_hash, 
                        G_context.signing_context.network_byte, (unsigned char *)G_context.signing_context.ui.line3);
                } else {
                    strncpy((char *)G_context.signing_context.ui.line3, d_app_rec.recipient.alias, 31);
                }
            }
            consume_pb_bytes(proto, field_total_size);
        } 
        // TAG 2: Function Call (Entry point for function name parsing)
        else if (tag == 2) { 
            proto->invoke_skip_remaining = (uint32_t)b_len;
            proto->waiting_for_func_name = true;
            proto->name_bytes_collected = 0; // Initialize capture counter
            consume_pb_bytes(proto, t_v + l_v); // Consume header and enter Level 3
        } 
        // TAG 3: Payments (Amount + AssetId)
        else if (tag == 3) { 
            if (proto->data_len < field_total_size) return false;
            waves_Amount am = waves_Amount_init_zero;
            am.asset_id.funcs.decode = &asset_callback_incremental;
            
            // Route asset ticker to line1 (1st payment) or line5 (2nd payment)
            if (proto->tx_invoke_count == 0) {
                am.asset_id.arg = G_context.signing_context.ui.line1;
            } else {
                am.asset_id.arg = G_context.signing_context.ui.line5;
            }

            pb_istream_t s = pb_istream_from_buffer(proto->data + t_v + l_v, (size_t)b_len);
            if (pb_decode(&s, waves_Amount_fields, &am)) {
                // Logic for the first payment
                if (proto->tx_invoke_count == 0) {
                    char decimals = G_context.signing_context.amount_decimals;
                    if (strlen((const char *)G_context.signing_context.ui.line1) == 0) {
                        memmove(G_context.signing_context.ui.line1, WAVES_CONST, 5);
                        decimals = 8; // Default for WAVES
                    }
                    print_amount(am.amount, decimals, (unsigned char *)&G_context.signing_context.ui.line4, 22);
                    proto->tx_invoke_count = 1;
                } 
                // Logic for the second payment
                else if (proto->tx_invoke_count == 1) {
                    char decimals = G_context.signing_context.amount2_decimals;
                    if (strlen((const char *)G_context.signing_context.ui.line5) == 0) {
                        memmove(G_context.signing_context.ui.line5, WAVES_CONST, 5);
                        decimals = 8;
                    }
                    print_amount(am.amount, decimals, (unsigned char *)&G_context.signing_context.ui.line6, 22);
                    proto->tx_invoke_count = 2;
                }
            }
            consume_pb_bytes(proto, field_total_size);
        } 
        else {
            // Skip unknown or unsupported tags
            consume_pb_bytes(proto, 1);
        }
    }
    return true;
}