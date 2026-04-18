#pragma once

#include <stddef.h>   // size_t
#include <stdbool.h>  // bool
#include <stdint.h>   // uint*_t

#include "buffer.h"

#include "types.h"

/**
 * Handler for GET_PUBLIC_KEY (INS 0x04). Parses BIP32 path, validates Waves path policy,
 * derives Ed25519 public key, optionally shows address on device.
 *
 * On success (silent path), builds APDU response: 32-byte pubkey + address length + Base58 address.
 *
 * @see G_context.bip32_path, G_context.pk_info.public_key, G_context.chain_id
 *
 * @param[in,out] cdata
 *   Command data with BIP32 path (depth + 5 x uint32 BE).
 * @param[in]     display
 *   If true, show address review; if false, send response immediately after derivation.
 * @param[in]     chain_id
 *   Network byte (P2), used for Waves address encoding.
 *
 * @return zero or positive on success (including async UI), negative on fatal I/O failure.
 */
int handler_get_public_key(buffer_t *cdata, bool display, uint8_t chain_id);
