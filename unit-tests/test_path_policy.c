/*
 * WAVES Ledger Application — unit tests for BIP32 path policy.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>

#include <cmocka.h>

#include "constants.h"
#include "crypto/path_policy.h"

static void test_valid_standard_path(void **state) {
    (void) state;
    uint32_t path[] = {
        0x8000002Cu,
        WAVES_SLIP44_COIN_TYPE_HARDENED,
        0x80000000u,
        0x80000000u,
        0x80000000u,
    };
    assert_true(waves_bip32_path_valid(path, WAVES_BIP32_DEPTH));
}

static void test_wrong_coin_type(void **state) {
    (void) state;
    uint32_t path[] = {
        0x8000002Cu,
        0x80000000u | 60u,
        0x80000000u,
        0x80000000u,
        0x80000000u,
    };
    assert_false(waves_bip32_path_valid(path, WAVES_BIP32_DEPTH));
}

static void test_non_hardened_account(void **state) {
    (void) state;
    uint32_t path[] = {
        0x8000002Cu,
        WAVES_SLIP44_COIN_TYPE_HARDENED,
        0u,
        0x80000000u,
        0x80000000u,
    };
    assert_false(waves_bip32_path_valid(path, WAVES_BIP32_DEPTH));
}

static void test_wrong_depth_len(void **state) {
    (void) state;
    uint32_t path5[] = {
        0x8000002Cu,
        WAVES_SLIP44_COIN_TYPE_HARDENED,
        0x80000000u,
        0x80000000u,
        0x80000000u,
    };
    assert_false(waves_bip32_path_valid(path5, 4));
}

static void test_null_path(void **state) {
    (void) state;
    assert_false(waves_bip32_path_valid(NULL, WAVES_BIP32_DEPTH));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_valid_standard_path),
        cmocka_unit_test(test_wrong_coin_type),
        cmocka_unit_test(test_non_hardened_account),
        cmocka_unit_test(test_wrong_depth_len),
        cmocka_unit_test(test_null_path),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
