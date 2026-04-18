#pragma once

#include <stddef.h>
#include <stdint.h>

#include "cx.h"

/** Streaming Blake2b-256; finalize with Keccak-256 (Waves SecureHash). */
typedef struct {
    cx_blake2b_t blake;
} waves_secure_hash_ctx_t;

cx_err_t waves_secure_hash_init(waves_secure_hash_ctx_t *ctx);

cx_err_t waves_secure_hash_update(waves_secure_hash_ctx_t *ctx,
                                  const uint8_t *data,
                                  size_t len);

/** Writes 32-byte SecureHash to @p out. */
cx_err_t waves_secure_hash_final(waves_secure_hash_ctx_t *ctx, uint8_t out[32]);

/** One-shot helper (tests / address hashing). */
cx_err_t waves_secure_hash_one_shot(const uint8_t *data, size_t len, uint8_t out[32]);
