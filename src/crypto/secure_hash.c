/*
 * WAVES Ledger Application — SecureHash (Keccak256(Blake2b256(data))).
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "secure_hash.h"

#include <string.h>

#include "cx.h"
#include "os.h"

/* lib_cxng syscall wrappers; not always prototyped in cx.h */
cx_err_t cx_blake2b_update(cx_blake2b_t *ctx, const uint8_t *data, size_t len);
cx_err_t cx_blake2b_final(cx_blake2b_t *ctx, uint8_t *digest);

cx_err_t waves_secure_hash_init(waves_secure_hash_ctx_t *ctx) {
    if (ctx == NULL) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    memset(&ctx->blake, 0, sizeof(ctx->blake));
    return cx_blake2b_init_no_throw(&ctx->blake, 256);
}

cx_err_t waves_secure_hash_update(waves_secure_hash_ctx_t *ctx,
                                  const uint8_t *data,
                                  size_t len) {
    if (ctx == NULL || (data == NULL && len > 0)) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    if (len == 0) {
        return CX_OK;
    }
    /* ledger-secure-sdk: no cx_blake2b_*_update/_final_no_throw; these return cx_err_t. */
    return cx_blake2b_update(&ctx->blake, data, len);
}

cx_err_t waves_secure_hash_final(waves_secure_hash_ctx_t *ctx, uint8_t out[32]) {
    if (ctx == NULL || out == NULL) {
        return CX_INVALID_PARAMETER_VALUE;
    }
    uint8_t blake_digest[32];
    cx_err_t err = cx_blake2b_final(&ctx->blake, blake_digest);
    if (err != CX_OK) {
        explicit_bzero(blake_digest, sizeof(blake_digest));
        return err;
    }
    err = cx_keccak_256_hash(blake_digest, sizeof(blake_digest), out);
    explicit_bzero(blake_digest, sizeof(blake_digest));
    return err;
}

cx_err_t waves_secure_hash_one_shot(const uint8_t *data, size_t len, uint8_t out[32]) {
    waves_secure_hash_ctx_t ctx;
    cx_err_t err = waves_secure_hash_init(&ctx);
    if (err != CX_OK) {
        explicit_bzero(&ctx, sizeof(ctx));
        return err;
    }
    err = waves_secure_hash_update(&ctx, data, len);
    if (err != CX_OK) {
        explicit_bzero(&ctx, sizeof(ctx));
        return err;
    }
    err = waves_secure_hash_final(&ctx, out);
    explicit_bzero(&ctx, sizeof(ctx));
    return err;
}
