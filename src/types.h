#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cx.h"

#include "constants.h"
#include "crypto/secure_hash.h"

typedef enum {
    STATE_NONE = 0,
    STATE_CONFIRM_ADDRESS,
    STATE_SIGN_STREAM_UI
} state_e;

typedef enum {
    SIGN_SESSION_IDLE = 0,
    SIGN_SESSION_STREAMING
} sign_session_e;

typedef struct {
    uint8_t public_key[WAVES_PUBKEY_LEN];
} pubkey_ctx_t;

typedef struct {
    waves_secure_hash_ctx_t hash_ctx;
    uint32_t total_expected;
    uint32_t received;
    uint8_t secure_hash[32];
    uint8_t signature[WAVES_SIG_LEN];
    sign_session_e session;
} sign_stream_ctx_t;

typedef struct {
    state_e state;
    uint32_t bip32_path[WAVES_BIP32_DEPTH];
    uint8_t bip32_path_len;
    /** Network byte from P2 (GET_PUBLIC_KEY / export address). */
    uint8_t chain_id;
    union {
        pubkey_ctx_t pk_info;
        sign_stream_ctx_t sign;
    };
    uint8_t req_display_address;
} global_ctx_t;
