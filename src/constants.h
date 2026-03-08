#pragma once

/**
 * Instruction class of the Boilerplate application.
 */
#define CLA 0x80

/**
 * Length of APPNAME variable in the Makefile.
 */
#define APPNAME_LEN (sizeof(APPNAME) - 1)

/**
 * Maximum length of MAJOR_VERSION || MINOR_VERSION || PATCH_VERSION.
 */
#define APPVERSION_LEN 3

/**
 * Maximum length of application name.
 */
#define MAX_APPNAME_LEN 64

/**
 * Maximum transaction length (bytes).
 */
#define MAX_TRANSACTION_LEN 510

/**
 * Maximum signature length (bytes).
 */
#define MAX_DER_SIG_LEN 72

/**
 * Exponent used to convert mBOL to BOL unit (N BOL = N * 10^3 mBOL).
 */
#define EXPONENT_SMALLEST_UNIT 3

/**
 * Boilerplate SLIP-44 coin type (TEST coin - 0x8001).
 * Production apps must use their assigned SLIP-44 coin type.
 * @see https://github.com/satoshilabs/slips/blob/master/slip-0044.md
 */
#define BOILERPLATE_SLIP44_COIN_TYPE 0x8001

/**
 * Boilerplate SLIP-44 coin type with hardened bit (0x80008001).
 */
#define BOILERPLATE_SLIP44_COIN_TYPE_HARDENED (0x80000000 | BOILERPLATE_SLIP44_COIN_TYPE)

#define SW_INCORRECT_STEP                     0x5000
#define SW_INCORRECT_P1_P2                    0x6A86
#define SW_CONDITIONS_NOT_SATISFIED           0x6985
#define SW_DEPRECATED_SIGN_PROTOCOL           0x9102
#define SW_INCORRECT_PRECISION_VALUE          0x9103
#define SW_INCORRECT_TRANSACTION_TYPE_VERSION 0x9104
#define SW_PROTOBUF_DECODING_FAILED           0x9105
#define SW_BYTE_DECODING_FAILED               0x9106
#define SW_HASHING_ERROR                      0x9107
#define PRESTO_DERIVE_ERROR                   0xFF00
#define INIT_PRIVATE_KEY_ERROR                0xFF01
#define INIT_PUBLIC_KEY_ERROR                 0xFF02
#define GENERATE_PAIR_ERROR                   0xFF03
#define D25519_TO_C25519_ERROR                0xFF04
#define SW_DEVICE_IS_LOCKED                   0x6986
#define SW_BUFFER_OVERFLOW                    0x6990
#define SW_INS_NOT_SUPPORTED                  0x6D00
#define SW_WRONG_DATA_LENGTH                  0x6A87

#define BYTE_DATA     1
#define PROTOBUF_DATA 2

#define P1_CONFIRM     0x01  // Show address confirmation
#define P1_NON_CONFIRM 0x00  // Don't show address confirmation
#define P1_LAST        0x80  // Parameter 1 = End of Bytes to Sign (finalize)
#define P1_MORE        0x00  // Parameter 1 = More bytes coming

#define SW_OK   0x9000
#define SW_DENY 0x9100