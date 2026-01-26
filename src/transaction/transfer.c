/*******************************************************************************
 * Waves Platform Wallet App for Nano Ledger devices
 * Copyright (c) 2017-2020 Sergey Tolmachev (Tolsi) <tolsi.ru@gmail.com>
 *
 * Based on Sample code provided (c) 2016 Ledger and
 * (c) 2017-2018 Jake B. (Burstcoin app)
 ********************************************************************************/

#include "transfer.h"
#include "buffer.h"
#include "../helper/buffer_helper.h"
#include "../globals.h"
#include "../handler/print_amount.h"
#include "../../crypto/waves.h"
#include "cx.h"

static bool try_to_fill_buffer_from_cdata(buffer_t *cdata) {
    size_t available = buffer_remaining(cdata);
    size_t needed = G_context.signing_context.ui.byte.wait_in_buffer;
    size_t to_copy = (available < needed) ? available : needed;

    // --- ЛОГИРОВАНИЕ ---
    PRINTF("--- Fill Buffer Start ---\n");
    PRINTF("Step: %d\n", G_context.signing_context.ui.byte.step);
    PRINTF("Internal Buffer Used: %d, Still Needed: %d\n", 
           G_context.signing_context.ui.byte.buffer_used, needed);
    
    // Показываем, что уже лежит в буфере (если там что-то есть с прошлого чанка)
    if (G_context.signing_context.ui.byte.buffer_used > 0) {
        PRINTF("Already in Buffer: %.*H\n", 
               G_context.signing_context.ui.byte.buffer_used, 
               G_context.signing_context.ui.byte.buffer);
    }

    // Показываем, что мы собираемся взять из входящего APDU
    if (to_copy > 0) {
        // buffer->ptr + buffer->offset — это текущая позиция в входящем APDU
        PRINTF("Copying %d bytes from CData (offset %d): %.*H\n", 
               to_copy, cdata->offset, to_copy, cdata->ptr + cdata->offset);
    }
    PRINTF("--- Fill Buffer End ---\n");
    // -------------------

    if (to_copy > 0) {
        if (!buffer_read_next(cdata, 
                             &G_context.signing_context.ui.byte.buffer[G_context.signing_context.ui.byte.buffer_used], 
                             to_copy)) {
            return false;
        }
        G_context.signing_context.ui.byte.buffer_used += to_copy;
        G_context.signing_context.ui.byte.wait_in_buffer -= to_copy;
    }
    return true;
}

void update_transfer_wait_in_buffer() {
    G_context.signing_context.ui.byte.buffer_used = 0;
    switch (G_context.signing_context.ui.byte.step) {
        case 2:
        case 4:
        case 9:
            G_context.signing_context.ui.byte.wait_in_buffer = 1;
            break;
        case 11:
            G_context.signing_context.ui.byte.wait_in_buffer = 3;
            break;
        case 0:    
        case 13:
            G_context.signing_context.ui.byte.wait_in_buffer = 2;
            break;
        case 6:
        case 7:
        case 8:
            G_context.signing_context.ui.byte.wait_in_buffer = 8;
            break;
        case 10:
            // use first byte from step 9
            G_context.signing_context.ui.byte.chunk_used -= 1;
            G_context.signing_context.ui.byte.wait_in_buffer = 26;
            break;
        case 1:
        case 3:
        case 5:
            G_context.signing_context.ui.byte.wait_in_buffer = 32;
            break;
        case 12:
            G_context.signing_context.ui.byte.wait_in_buffer =
                G_context.signing_context.ui.byte.alias_size;
            break;
        case 14:
            G_context.signing_context.ui.byte.wait_in_buffer =
                G_context.signing_context.ui.byte.attachment_size;
            break;
        case 15:
            G_context.signing_context.ui.byte.wait_in_buffer = 0;
            break;
    }
    PRINTF("Next step prepared: %d, waiting for %d bytes\n", 
           G_context.signing_context.ui.byte.step, 
           G_context.signing_context.ui.byte.wait_in_buffer);
}

cx_err_t build_transfer_ui_step(buffer_t *cdata) {
    PRINTF("build_transfer_ui_step: step=%d, wait=%d, cdata_rem=%d\n", 
            G_context.signing_context.ui.byte.step, 
            G_context.signing_context.ui.byte.wait_in_buffer,
            buffer_remaining(cdata));

    if (G_context.signing_context.ui.byte.step == 0) {
        update_transfer_wait_in_buffer();
    }        
    // Fill internal buffer, if data is not enough for current step
    if (G_context.signing_context.ui.byte.wait_in_buffer > 0) {
        if(!try_to_fill_buffer_from_cdata(cdata)) {
            return SW_OK;
        }
    }

    // If we have enough bytes for current step of the state machine
    if (G_context.signing_context.ui.byte.wait_in_buffer == 0 &&
        !G_context.signing_context.ui.finished) {
        
        size_t length = 45;
        bool is_flag_set = G_context.signing_context.ui.byte.buffer[0] == 1;
        uint64_t amount = 0;
        uint64_t fee = 0;

        PRINTF("PROCESSING step %d, Buffer: %.*H\n", 
               G_context.signing_context.ui.byte.step, 
               G_context.signing_context.ui.byte.buffer_used, 
               G_context.signing_context.ui.byte.buffer);

        switch (G_context.signing_context.ui.byte.step) {
            case 0:
                PRINTF("Step 0: Checking Header. Buffer: %.*H\n", 
                            G_context.signing_context.ui.byte.buffer_used, 
                            G_context.signing_context.ui.byte.buffer);

                // Проверка типа (должен быть 0x04 для Transfer)
                if (G_context.signing_context.ui.byte.buffer[0] != 0x04) {
                    PRINTF("Error: Invalid Transaction Type %02X\n", G_context.signing_context.ui.byte.buffer[0]);
                    return SW_CONDITIONS_NOT_SATISFIED;
                }

                // Если версия 2, проверяем и её
                if (G_context.signing_context.data_version == 2) {
                    if (G_context.signing_context.ui.byte.buffer[1] != 0x02) {
                        PRINTF("Error: Invalid Version %02X\n", G_context.signing_context.ui.byte.buffer[1]);
                        return SW_CONDITIONS_NOT_SATISFIED;
                    }
                }

                PRINTF("Step 0: Header OK. Moving to Step 1\n");
                G_context.signing_context.ui.byte.step = 1;
                update_transfer_wait_in_buffer();
                break;

            case 1:
                PRINTF("Step 1: Sender PubKey saved\n");
                memmove(&G_context.signing_context.ui.from,
                        G_context.signing_context.ui.byte.buffer,
                        32);
                G_context.signing_context.ui.byte.step = 2;
                update_transfer_wait_in_buffer();
                break;

            case 2:
                PRINTF("Step 2: Amount asset flag = %d\n", is_flag_set);
                if (is_flag_set) {
                    G_context.signing_context.ui.byte.step = 3;
                } else {
                    memmove((char *) G_context.signing_context.ui.line2, WAVES_CONST, 5);
                    G_context.signing_context.ui.byte.step = 4;
                }
                update_transfer_wait_in_buffer();
                break;

            case 3:
                PRINTF("Step 3: Asset ID b58 encoding\n");
                if (!b58enc((char *) G_context.signing_context.ui.line2,
                            &length,
                            (const void *) G_context.signing_context.ui.byte.buffer,
                            32)) {
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                G_context.signing_context.ui.byte.step = 4;
                update_transfer_wait_in_buffer();
                break;

            case 4:
                PRINTF("Step 4: Fee asset flag = %d\n", is_flag_set);
                if (is_flag_set) {
                    G_context.signing_context.ui.byte.step = 5;
                } else {
                    memmove((char *) G_context.signing_context.ui.fee_asset, WAVES_CONST, 5);
                    G_context.signing_context.ui.byte.step = 6;
                }
                update_transfer_wait_in_buffer();
                break;

            case 5:
                PRINTF("Step 5: Fee asset ID b58 encoding\n");
                if (!b58enc((char *) G_context.signing_context.ui.fee_asset,
                            &length,
                            (const void *) G_context.signing_context.ui.byte.buffer,
                            32)) {
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                G_context.signing_context.ui.byte.step = 6;
                update_transfer_wait_in_buffer();
                break;

            case 6:
                PRINTF("Step 6: Timestamp skipped\n");
                G_context.signing_context.ui.byte.step = 7;
                update_transfer_wait_in_buffer();
                break;

            case 7:
                copy_in_reverse_order((unsigned char *) &amount,
                                      G_context.signing_context.ui.byte.buffer,
                                      8);
                PRINTF("Step 7: Amount raw = %.*H\n", 8, G_context.signing_context.ui.byte.buffer);
                if (!print_amount(amount,
                                 G_context.signing_context.amount_decimals,
                                 (unsigned char *) G_context.signing_context.ui.line1,
                                 20)) {
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                G_context.signing_context.ui.byte.step = 8;
                update_transfer_wait_in_buffer();
                break;

            case 8:
                copy_in_reverse_order((unsigned char *) &fee,
                                      G_context.signing_context.ui.byte.buffer,
                                      8);
                PRINTF("Step 8: Fee raw = %.*H\n", 8, G_context.signing_context.ui.byte.buffer);
                if (!print_amount(fee,
                                 G_context.signing_context.fee_decimals,
                                 (unsigned char *) G_context.signing_context.ui.fee_amount,
                                 20)) {
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                G_context.signing_context.ui.byte.step = 9;
                update_transfer_wait_in_buffer();
                break;

            case 9:
                PRINTF("Step 9: Recipient Flag = %02X\n", G_context.signing_context.ui.byte.buffer[0]);
                if (G_context.signing_context.ui.byte.buffer[0] == 1) {
                    G_context.signing_context.ui.byte.step = 10;
                } else if (G_context.signing_context.ui.byte.buffer[0] == 2) {
                    G_context.signing_context.ui.byte.step = 11;
                } else {
                    PRINTF("Step 9 ERROR: Invalid recipient flag!\n");
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                update_transfer_wait_in_buffer();
                break;

            case 10:
                PRINTF("Step 10: Address encoding\n");
                if (!b58enc((char *) G_context.signing_context.ui.line3,
                            &length,
                            (const void *) G_context.signing_context.ui.byte.buffer,
                            26)) {
                    return SW_CONDITIONS_NOT_SATISFIED;
                }
                G_context.signing_context.ui.byte.step = 13;
                update_transfer_wait_in_buffer();
                break;

            case 11:
                G_context.signing_context.ui.byte.alias_size =
                    G_context.signing_context.ui.byte.buffer[1] << 8 |
                    G_context.signing_context.ui.byte.buffer[2];
                PRINTF("Step 11: Alias size = %d\n", G_context.signing_context.ui.byte.alias_size);
                G_context.signing_context.ui.byte.step = 12;
                update_transfer_wait_in_buffer();
                break;

            case 12:
                PRINTF("Step 12: Alias string saved\n");
                if (G_context.signing_context.ui.byte.alias_size > 30 ||
                    G_context.signing_context.ui.byte.alias_size < 4) {
                    return SW_BYTE_DECODING_FAILED;
                }
                memmove((unsigned char *) G_context.signing_context.ui.line3,
                        G_context.signing_context.ui.byte.buffer,
                        G_context.signing_context.ui.byte.alias_size);
                G_context.signing_context.ui.byte.step = 13;
                update_transfer_wait_in_buffer();
                break;

            case 13:
                copy_in_reverse_order(
                    (unsigned char *) &G_context.signing_context.ui.byte.attachment_size,
                    G_context.signing_context.ui.byte.buffer,
                    2);
                PRINTF("Step 13: Attachment size = %d\n", G_context.signing_context.ui.byte.attachment_size);
                G_context.signing_context.ui.byte.step = 14;
                update_transfer_wait_in_buffer();
                break;

            case 14:
                PRINTF("Step 14: Attachment data processing\n");
                {
                    uint16_t actual_size = G_context.signing_context.ui.byte.attachment_size;
                    if (actual_size > 41) {
                        memmove((unsigned char *) &G_context.signing_context.ui.line4[41],
                                "...\0",
                                4);
                        actual_size = 41;
                    }
                    memmove((unsigned char *) G_context.signing_context.ui.line4,
                            G_context.signing_context.ui.byte.buffer,
                            actual_size);
                }
                G_context.signing_context.ui.byte.step = 15;
                update_transfer_wait_in_buffer();
                break;

            case 15:
                PRINTF("Step 15: FINISHED\n");
                G_context.signing_context.ui.finished = true;
                G_context.signing_context.step = 7;
                update_transfer_wait_in_buffer();
                break;

            default:
                PRINTF("CRITICAL ERROR: Step %d is undefined!\n", G_context.signing_context.ui.byte.step);
                return INVALID_COUNTER;
        }
    }
    return CX_OK;
}