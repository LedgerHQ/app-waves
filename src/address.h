#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "constants.h"

/** Max Base58 address string for Waves (including NUL for internal buffers). */
#define WAVES_ADDRESS_STR_MAX 48

/**
 * Build Waves account address (Base58) from 32-byte Ed25519 public key.
 * @param chain_id network byte (e.g. 'W' mainnet)
 */
bool waves_encode_address(uint8_t chain_id,
                          const uint8_t pubkey[WAVES_PUBKEY_LEN],
                          char *out,
                          size_t out_sz);

/** Writes a short network description into @p buf (NUL-terminated). */
void waves_format_network_line(uint8_t chain_id, char *buf, size_t buf_sz);
