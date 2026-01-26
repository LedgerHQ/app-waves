// /*****************************************************************************
//  *   Ledger App Boilerplate.
//  *   (c) 2020 Ledger SAS.
//  *
//  *  Licensed under the Apache License, Version 2.0 (the "License");
//  *  you may not use this file except in compliance with the License.
//  *  You may obtain a copy of the License at
//  *
//  *      http://www.apache.org/licenses/LICENSE-2.0
//  *
//  *  Unless required by applicable law or agreed to in writing, software
//  *  distributed under the License is distributed on an "AS IS" BASIS,
//  *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  *  See the License for the specific language governing permissions and
//  *  limitations under the License.
//  *****************************************************************************/

#include <stdint.h>   // uint*_t
#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <string.h>   // memset, explicit_bzero

#include "os.h"
#include "cx.h"
#include "buffer.h"
#include "buffer_helper.h"
#include "sw.h"
#include "globals.h"
#include "transfer.h"
#include "protobuf.h"
#include "pb_parse.h"
#include "tx_types.h"
#include "sign_utils.h"
#include "../crypto/ledger_crypto.h"

cx_err_t make_allowed_sign_steps(buffer_t *cdata) {
    uint8_t chunk_data_size = cdata->size;
    cx_err_t error = CX_OK;

    if (G_context.signing_context.chunk == 0) {
        chunk_data_size -= 29;

        if (!read_path_from_buffer(cdata, (uint32_t *) G_context.signing_context.bip32, 5)) {
            return io_send_sw(SW_CONDITIONS_NOT_SATISFIED);
        }

        // 7 bytes tx info
        buffer_read_u8(cdata, &G_context.signing_context.amount_decimals);
        buffer_read_u8(cdata, &G_context.signing_context.amount2_decimals);
        buffer_read_u8(cdata, &G_context.signing_context.fee_decimals);
        buffer_read_u8(cdata, &G_context.signing_context.data_type);
        buffer_read_u8(cdata, &G_context.signing_context.data_version);
        buffer_read_u32(cdata, &G_context.signing_context.data_size, BE);

        G_context.signing_context.message_type = getMessageType();

        if (G_context.signing_context.amount_decimals < 0 ||
            G_context.signing_context.amount_decimals > 8) {
            return SW_INCORRECT_PRECISION_VALUE;
        }
        if (G_context.signing_context.amount2_decimals < 0 ||
            G_context.signing_context.amount2_decimals > 8) {
            return SW_INCORRECT_PRECISION_VALUE;
        }
    }

    while (G_context.signing_context.chunk_used < chunk_data_size &&
           G_context.signing_context.step < 5) {
        error = make_sign_step(chunk_data_size, cdata);
        if (error != CX_OK) {
            return error;
        }
    }
    return CX_OK;
    // else wait for next chunk
}

uint32_t deserialize_uint32_t(unsigned char *buffer) {
    uint32_t value = 0;

    value |= buffer[0] << 24;
    value |= buffer[1] << 16;
    value |= buffer[2] << 8;
    value |= buffer[3];
    return value;
}

void read_path_from_bytes(unsigned char *buffer, uint32_t *path) {
    path[0] = deserialize_uint32_t(buffer);
    path[1] = deserialize_uint32_t(buffer + 4);
    path[2] = deserialize_uint32_t(buffer + 8);
    path[3] = deserialize_uint32_t(buffer + 12);
    path[4] = deserialize_uint32_t(buffer + 16);
}

int getMessageType() {
    unsigned char tx_type = G_context.signing_context.data_type;
    unsigned char tx_ver = G_context.signing_context.data_version;
    // just one step here
    if (tx_type == 3) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 4) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 5) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 6) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 8) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 9) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 10) {
        if (tx_ver <= 2) {
            return BYTE_DATA;
        } else if (tx_ver >= 3) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 11) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 12) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 13) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 14) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 15) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 16) {
        if (tx_ver == 1) {
            return BYTE_DATA;
        } else if (tx_ver >= 2) {
            return PROTOBUF_DATA;
        }
    } else if (tx_type == 17) {
        return PROTOBUF_DATA;
    } else if (tx_type > 200) {
        if (tx_type == 252) {
            if (tx_ver <= 3) {
                return BYTE_DATA;
            } else if (tx_ver >= 4) {
                return PROTOBUF_DATA;
            }
        } else if (tx_type == 253) {
            return BYTE_DATA;
        } else if (tx_type == 254) {
            return BYTE_DATA;
        } else if (tx_type == 255) {
            return BYTE_DATA;
        } else {
            return BYTE_DATA;
        }
    }
    return 0;
}

cx_err_t sign_step1() {
    if (G_context.signing_context.step != 1) {
        return SW_INCORRECT_STEP;
    }

    cx_ecfp_public_key_t public_key;
    cx_ecfp_private_key_t private_key;

    cx_err_t error = get_keypair_by_path((uint32_t *) G_context.signing_context.bip32,
                                         &public_key,
                                         &private_key);

    if (error != CX_OK) {
        return error;
    }

    error = stream_eddsa_sign_step1(&G_context.signing_context.eddsa_context, &private_key);

    if (error != CX_OK) {
        // Clear secret
        explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));
        return error;
    }

    public_key_le_to_be(&public_key);
    G_context.signing_context.sign_bit = public_key.W[31] & 0x80;
    // Clear secret after use
    explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));
    explicit_bzero(&public_key, sizeof(cx_ecfp_public_key_t));
    G_context.signing_context.step = 2;
    return CX_OK;
}

cx_err_t sign_step2(uint8_t chunk_data_size, buffer_t *cdata) {
    if (G_context.signing_context.step != 2) {
        return SW_INCORRECT_STEP;
    }

    PRINTF("sign_step2: chunk_data_size=%d\n", chunk_data_size);
    cx_err_t error = CX_OK;

    if (G_context.signing_context.data_read < G_context.signing_context.data_size) {
        error = hash_stream_data(chunk_data_size, cdata);
        if (error != CX_OK) {
            return error;
        }
    } else {
        error = stream_eddsa_sign_step3(&G_context.signing_context.eddsa_context);
        if (error != CX_OK) {
            return error;
        }
        G_context.signing_context.step = 4;
        G_context.signing_context.data_read = 0;
    }
    PRINTF("sign_step2: step=%d data_read=%d data_size=%d\n",
           G_context.signing_context.step,
           G_context.signing_context.data_read,
           G_context.signing_context.data_size);
    return CX_OK;
}

cx_err_t sign_step3(uint8_t chunk_data_size, buffer_t *cdata) {
    if (G_context.signing_context.step != 4) {
        return SW_INCORRECT_STEP;
    }

    PRINTF("sign_step3: chunk_data_size=%d\n", chunk_data_size);

    if (G_context.signing_context.data_read < G_context.signing_context.data_size) {
        cx_err_t error = hash_stream_data(chunk_data_size, cdata);
        // not call make_sign_step() because G_context.signing_context.chunk_used <
        // chunk_data_size will be false
        if (error != CX_OK) {
            return error;
        }
        if (G_context.signing_context.data_read == G_context.signing_context.data_size) {
            G_context.signing_context.step = 5;
        }
    }
    PRINTF("sign_step3: step=%d data_read=%d data_size=%d\n",
           G_context.signing_context.step,
           G_context.signing_context.data_read,
           G_context.signing_context.data_size);
    return CX_OK;
}

cx_err_t make_sign_step(uint8_t chunk_data_size, buffer_t *cdata) {
    if (G_context.signing_context.step == 1) {
        return sign_step1();
    } else if (G_context.signing_context.step == 2) {
        return sign_step2(chunk_data_size, cdata);
    } else if (G_context.signing_context.step == 4) {
        sign_step3(chunk_data_size, cdata);
    }
    return CX_OK;
}

cx_err_t hash_stream_data(uint8_t chunk_data_size, buffer_t *cdata) {
    // 1. Calculate, how many bytes in this chunk we haven't processed yet
    // chunk_data_size — this is the total size of the payload in the current APDU
    uint32_t chunk_data_left = chunk_data_size - G_context.signing_context.chunk_used;

    // 2. Calculate, how many bytes of the transaction are left to read
    uint32_t data_read_left =
        G_context.signing_context.data_size - G_context.signing_context.data_read;

    // 3. Handling skipping of initial bytes (if this is the start of the transaction)
    if (G_context.signing_context.data_read == 0) {
        uint8_t to_skip = G_context.signing_context.sign_from;

        // Shift buffer cursor to skip "unnecessary" bytes
        if (!buffer_seek_next(cdata, to_skip)) {
            return CX_INTERNAL_ERROR;  // В буфере меньше байт, чем мы хотим пропустить
        }

        chunk_data_left -= to_skip;
        data_read_left -= to_skip;
        G_context.signing_context.chunk_used += to_skip;
    }

    // 4. Determine how many bytes we can feed to the hash function now
    uint32_t step_read_bytes_left = MIN(chunk_data_left, data_read_left);

    if (step_read_bytes_left > 0) {
        // Get a direct pointer to the current position in the buffer WITHOUT copying
        // This is more efficient for streaming hashing

        cx_err_t error = stream_eddsa_sign_step2(&G_context.signing_context.eddsa_context,
                                                 cdata->ptr + cdata->offset,
                                                 step_read_bytes_left);

        if (error != CX_OK) {
            return error;
        }

        // 5. Move cursor in buffer after successful hashing
        if (!buffer_seek_next(cdata, step_read_bytes_left)) {
            return CX_INTERNAL_ERROR;
        }
    }

    // 6. Update global state
    G_context.signing_context.data_read += step_read_bytes_left;
    G_context.signing_context.chunk_used += step_read_bytes_left;

    return CX_OK;
}

cx_err_t make_allowed_ui_steps(buffer_t *cdata, bool is_last) {
    PRINTF("make_allowed_ui_steps start\n Is protobuf: %d\n",
           G_context.signing_context.message_type == PROTOBUF_DATA);

    if (G_context.signing_context.message_type == PROTOBUF_DATA) {
        PRINTF("Parse PROTOBUF DATA\n");

        // Проверка на устаревший протокол
        if (is_last && cdata->size == G_context.signing_context.chunk_used &&
            G_context.signing_context.step == 6) {
            return SW_DEPRECATED_SIGN_PROTOCOL;
        }

        if (!G_context.signing_context.ui.finished) {
            // В первом чанке пропускаем заголовок опций (29 байт)
            if (G_context.signing_context.chunk == 0) {
                if (!buffer_seek_next(cdata, 29)) return SW_WRONG_DATA_LENGTH;
            }

            // Оборачиваем ВСЮ логику Protobuf в ОДИН TRY-CATCH блок
            // Это решает проблему ошибки "redefinition of label __FINALLYEX"
            BEGIN_TRY {
                TRY {
                    if (G_context.signing_context.data_type == 252) {
                        // --- ОБРАБОТКА ОРДЕРА ---
                        if (G_context.signing_context.step < 7) {
                            bool finished = pb_incremental_parse_order(
                                &G_context.signing_context.ui,
                                cdata,
                                G_context.signing_context.data_size);

                            if (finished) {
                                PRINTF("Order parsing finished, hash matched.\n");
                                G_context.signing_context.step = 8;
                            }
                        }
                    } else {
                        // --- ОБРАБОТКА ТРАНЗАКЦИИ ---
                        if (G_context.signing_context.step < 7) {
                            PRINTF("Parsing Root Header...\n");
                            if (build_protobuf_root_tx_incremental(
                                    &G_context.signing_context.ui,
                                    cdata,
                                    G_context.signing_context.data_size)) {
                                PRINTF("Parsing pb tx DONE\n");
                                G_context.signing_context.step = 8;
                            }
                        }
                    }
                }
                CATCH_OTHER(e) {
                    PRINTF("PB Exception: %d\n", e);
                    CLOSE_TRY; 
                    return (e == 0) ? SW_PROTOBUF_DECODING_FAILED : e;
                }
                FINALLY {
                    // Если это последний чанк и парсинг завершен (step 8),
                    // разрешаем отображение UI
                    if (is_last && G_context.signing_context.step == 8) {
                        G_context.signing_context.ui.finished = true;
                    }
                }
            }
            END_TRY;

            return CX_OK;
        } else {
            return SW_INS_NOT_SUPPORTED;
        }
    } else {
        // --- HANDLE NOT-PROTOBUF DATA (legacy bytes) ---
        if (G_context.signing_context.ui.byte.step == 0 && G_context.signing_context.step == 6) {
            if (is_last && cdata->size == G_context.signing_context.chunk_used) {
                return SW_DEPRECATED_SIGN_PROTOCOL;
            }
        }

        if (G_context.signing_context.step == 6) {
            if (G_context.signing_context.data_type == 4) {
                while ((buffer_remaining(cdata) > 0 && G_context.signing_context.ui.byte.step < 15) ||
                       (G_context.signing_context.ui.byte.step == 15 && !G_context.signing_context.ui.finished)) {
                    int error = build_transfer_ui_step(cdata);
                    if (error) {
                        PRINTF("Error build_ui_step step %d\n", error);
                        return error;
                    }
                }
            } else {
                G_context.signing_context.ui.byte.step++;
            }

            size_t hash_data_size = buffer_remaining(cdata);
            bool last_hash = false;

            if (G_context.signing_context.ui.byte.total_received + hash_data_size >= G_context.signing_context.data_size) {
                hash_data_size = G_context.signing_context.data_size - G_context.signing_context.ui.byte.total_received;
                last_hash = true;
                G_context.signing_context.ui.byte.total_received = 0;
                if (G_context.signing_context.data_type != 4) {
                    build_other_data_ui();
                    G_context.signing_context.step = 7;
                }
            }

            G_context.signing_context.ui.byte.total_received += hash_data_size;

            if (cx_hash_no_throw(&G_context.signing_context.ui.hash_ctx.header,
                                 CX_NONE,
                                 cdata->ptr + cdata->offset,
                                 hash_data_size,
                                 NULL, 0)) {
                return SW_HASHING_ERROR;
            }

            if (last_hash) {
                memset(&G_context.signing_context.ui.byte.buffer, 0, 64);
                if (cx_hash_no_throw(&G_context.signing_context.ui.hash_ctx.header,
                                     CX_LAST,
                                     NULL, 0,
                                     G_context.signing_context.ui.byte.buffer, 32)) {
                    return SW_HASHING_ERROR;
                }

                if (memcmp(&G_context.signing_context.first_data_hash,
                           &G_context.signing_context.ui.byte.buffer, 32) != 0) {
                    return SW_SIGN_DATA_NOT_MATCH;
                }
            }
        }
        
        if (is_last) {
            G_context.signing_context.step = 8;
        }
        G_context.signing_context.ui.byte.chunk_used = 0;
    }

    PRINTF("make_allowed_ui_steps end\n");
    return CX_OK;
}

cx_err_t build_other_data_ui() {
    unsigned char tx_type = G_context.signing_context.data_type;
    // just one step here
    memmove(&G_context.signing_context.ui.line2, &"Transaction Id\0", 15);
    if (tx_type == 3) {
        memmove(&G_context.signing_context.ui.line1, &"issue\0", 6);
    } else if (tx_type == 4) {
        memmove(&G_context.signing_context.ui.line1, &"transfer\0", 9);
    } else if (tx_type == 5) {
        memmove(&G_context.signing_context.ui.line1, &"reissue\0", 8);
    } else if (tx_type == 6) {
        memmove(&G_context.signing_context.ui.line1, &"burn\0", 5);
    } else if (tx_type == 8) {
        memmove(&G_context.signing_context.ui.line1, &"start leasing\0", 14);
    } else if (tx_type == 9) {
        memmove(&G_context.signing_context.ui.line1, &"cancel leasing\0", 15);
    } else if (tx_type == 10) {
        memmove(&G_context.signing_context.ui.line2, &"Transaction Hash\0", 17);
        memmove(&G_context.signing_context.ui.line1, &"creating an alias\0", 18);
    } else if (tx_type == 11) {
        memmove(&G_context.signing_context.ui.line1, &"mass transfer\0", 14);
    } else if (tx_type == 12) {
        memmove(&G_context.signing_context.ui.line1, &"data\0", 5);
    } else if (tx_type == 13) {
        memmove(&G_context.signing_context.ui.line1, &"set script\0", 11);
    } else if (tx_type == 14) {
        memmove(&G_context.signing_context.ui.line1, &"sponsorship\0", 12);
    } else if (tx_type == 15) {
        memmove(&G_context.signing_context.ui.line1, &"asset script\0", 13);
    } else if (tx_type == 16) {
        memmove(&G_context.signing_context.ui.line1, &"script invocation\0", 18);
    } else if (tx_type == 17) {
        memmove(&G_context.signing_context.ui.line1, &"update asset info\0", 18);
    } else if (tx_type > 200) {
        // type byte >200 are 'reserved', it will not be signed
        memmove(&G_context.signing_context.ui.line2, &"Hash\0", 5);
        if (tx_type == 252) {
            memmove(&G_context.signing_context.ui.line1, &"order\0", 6);
        } else if (tx_type == 253) {
            memmove(&G_context.signing_context.ui.line1, &"data\0", 5);
        } else if (tx_type == 254) {
            memmove(&G_context.signing_context.ui.line1, &"request\0", 8);
        } else if (tx_type == 255) {
            memmove(&G_context.signing_context.ui.line1, &"message\0", 8);
        } else {
            memmove(&G_context.signing_context.ui.line1, &"something\0", 10);
        }
    }

    if (strlen((const char *) G_context.signing_context.ui.line1) == 0) {
        memmove(&G_context.signing_context.ui.line1, &"transaction\0", 12);
    }

    // Get the public key and return it.
    cx_ecfp_public_key_t public_key;

    if (!get_curve25519_public_key_for_path((uint32_t *) G_context.signing_context.bip32,
                                            &public_key)) {
        return INVALID_PARAMETER;
    }

    memmove(&G_context.signing_context.ui.from, public_key.W, 32);
    G_context.signing_context.ui.finished = true;
    return CX_OK;
}