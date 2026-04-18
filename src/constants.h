#pragma once

#include <stdint.h>

/** APDU class (Waves protocol). */
#define CLA 0xE0

#ifndef MAJOR_VERSION
#define MAJOR_VERSION 2
#endif
#ifndef MINOR_VERSION
#define MINOR_VERSION 1
#endif
#ifndef PATCH_VERSION
#define PATCH_VERSION 0
#endif

#define APPNAME_LEN (sizeof(APPNAME) - 1)

#define APPVERSION_LEN 3

#define MAX_APPNAME_LEN 64

/** Ed25519 public key length (Waves account key). */
#define WAVES_PUBKEY_LEN 32

/** Ed25519 signature length. */
#define WAVES_SIG_LEN 64

/** BIP32 path depth for Waves standard path (5 levels). */
#define WAVES_BIP32_DEPTH 5

/** Serialized BIP32 path: 5 x uint32 BE. */
#define WAVES_BIP32_PATH_BYTES (WAVES_BIP32_DEPTH * sizeof(uint32_t))

/**
 * Maximum streamed transaction size (bytes) the app accepts.
 * Host must chunk; this caps state-machine abuse.
 */
#define MAX_TX_STREAM_TOTAL (16u * 1024u * 1024u)

/** GET_APP_CONFIGURATION response: blind signing capable. */
#define APP_CONFIG_FLAG_BLIND_SIGN 0x01

/** Utility INS: version triplet (boilerplate-style). */
#define INS_GET_VERSION 0x03
/** Spec: GET_PUBLIC_KEY. */
#define INS_GET_PUBLIC_KEY 0x04
/** Spec: GET_APP_CONFIGURATION. */
#define INS_GET_APP_CONFIGURATION 0x06
/** Spec: SIGN_TX_STREAM. */
#define INS_SIGN_TX_STREAM 0x08
/** Utility INS: raw app name string. */
#define INS_GET_APP_NAME_UTIL 0xF4

#define P1_GET_PK_SILENT     0x00
#define P1_GET_PK_CONFIRM    0x01

#define P1_SIGN_INIT 0x00
#define P1_SIGN_ADD  0x01
#define P1_SIGN_LAST 0x80

/** SLIP-44 WAVES coin type (decimal). */
#define WAVES_SLIP44_COIN_TYPE            5741564u
#define WAVES_SLIP44_COIN_TYPE_HARDENED   (0x80000000u | WAVES_SLIP44_COIN_TYPE)
