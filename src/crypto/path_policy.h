/*
 * WAVES Ledger Application — BIP32 path policy (spec §2.2).
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * True if @p path matches Waves standard policy: depth 5, 44', 5741564',
 * and hardened account / 0' / address_index'.
 */
bool waves_bip32_path_valid(const uint32_t *path, size_t len);
