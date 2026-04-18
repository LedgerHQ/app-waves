#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "buffer.h"

static void test_parse_sign_init_prefix(void **state) {
    (void) state;
    /* 5 levels: 44'/5741564'/0'/0'/0' + total_be=5 + chunk "abc" */
    uint8_t raw[] = {
        0x05,
        0x80,
        0x00,
        0x00,
        0x2c,
        0x80,
        0x57,
        0x9b,
        0xfc,
        0x80,
        0x00,
        0x00,
        0x00,
        0x80,
        0x00,
        0x00,
        0x00,
        0x80,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x05,
        0x61,
        0x62,
        0x63,
    };
    buffer_t b = {.ptr = raw, .size = sizeof(raw), .offset = 0};
    uint8_t n = 0;
    uint32_t p[5];
    uint32_t tot = 0;
    assert_true(buffer_read_u8(&b, &n));
    assert_int_equal(n, 5);
    assert_true(buffer_read_bip32_path(&b, p, 5));
    assert_true(buffer_read_u32(&b, &tot, BE));
    assert_int_equal(tot, 5);
    assert_int_equal(b.size - b.offset, 3);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_parse_sign_init_prefix),
    };
    return cmocka_run_group_tests(tests, NULL, NULL);
}
