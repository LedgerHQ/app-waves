#pragma once

#include <stddef.h>  // size_t
#include <stdint.h>  // uint*_t

#include "bip32.h"
#include "./crypto/stream_eddsa_sign.h"
#include "constants.h"
#include "tx_types.h"
#include "cx.h"
#include "nanopb/pb.h"
#include "nanopb/pb_decode.h"
#include "nanopb_stubs/order.pb.h"
#include "nanopb_stubs/transaction.pb.h"

/**
 * Enumeration with expected INS of APDU commands.
 */
typedef enum {
    GET_APP_AND_VERSION = 0x01,  /// get app name and version
    INS_SIGN = 0x02,
    GET_PUBLIC_KEY = 0x04,   /// public key of corresponding BIP32 path
    GET_APP_VERSION = 0x06,  /// version of the application
    GET_APP_NAME = 0x08,     /// name of the application
} command_e;
/**
 * Enumeration with parsing state.
 */
typedef enum {
    STATE_NONE,      /// No state
    STATE_PARSED,    /// Transaction data parsed
    STATE_APPROVED,  /// Transaction data approved
    STATE_REJECTED   /// Transaction data rejected
} state_e;

/**
 * Enumeration with user request type.
 */
typedef enum {
    CONFIRM_ADDRESS,           /// confirm address derived from public key
    CONFIRM_TRANSACTION,       /// confirm transaction information
    CONFIRM_TOKEN_TRANSACTION  /// confirm token transaction information
} request_type_e;

/**
 * Structure for public key context information.
 */
typedef struct {
    uint8_t raw_public_key[65];  /// format (1), x-coordinate (32), y-coodinate (32)
    uint8_t chain_code[32];      /// for public key derivation
} pubkey_ctx_t;

#define TOKEN_ADDRESS_LEN 32

/**
 * Structure for token information.
 */
typedef struct {
    const char *ticker;  /// token ticker
    uint8_t decimals;    /// number of decimals
} token_info_t;

typedef struct uiByte_t {
    unsigned char buffer[150];
    uint8_t step;
    uint8_t wait_in_buffer;
    uint8_t buffer_used;
    uint32_t chunk_used;
    uint32_t total_received;
    uint16_t alias_size;
    uint16_t attachment_size;
} uiByte_t;

// Типы Wire Type из спецификации Protobuf
#define PB_WT_VARINT 0
#define PB_WT_STRING 2

typedef enum {
    PB_STATE_ROOT,
    PB_STATE_SUBSTREAM,
    PB_STATE_SKIP
} pb_parser_state_t;

typedef struct uiProtobuf_t {
    uint16_t data_len;
    
    uint32_t tx_invoke_count;   
    uint32_t name_total_expected;   // Общая длина имени из Protobuf
    uint32_t name_bytes_collected;  // Сколько уже скопировали в ctx->line2
    uint32_t invoke_skip_remaining; // Оставшийся размер всего поля FunctionCall (Tag 2)
    bool waiting_for_func_name;
    uint8_t name_buffer[45]; 

    pb_istream_t stream;      // Состояние стрима Nanopb
    bool initialized;
    uint32_t total_size;
    uint32_t total_received;
    uint32_t total_read;
    uint32_t read_offset;
    uint32_t bytes_stored;
    uint32_t skip_remaining;
    uint32_t sub_stream_remaining;
    uint32_t sub_remaining;
    uint32_t pending_bytes;
    pb_parser_state_t state;
    uint32_t current_sub_tag;
    uint32_t current_data_tag;
    cx_blake2b_t hash_ctx;   
    waves_Transaction tx; 
    uint8_t data[128];         // Буфер для текущего тега/маленького объекта
} uiProtobuf_t;

// A place to store information about the transaction
// for displaying to the user when requesting approval.
// 44 bytes for address/id + 1 for null terminator.
typedef struct uiContext_t {
    union {
        uiByte_t byte;       // Context for legacy (non-protobuf) binary format
        uiProtobuf_t proto;  // Context for new Protobuf format
    };
    unsigned char from[36];
    unsigned char fee_asset[45];
    unsigned char line1[45];
    unsigned char line2[45];
    unsigned char line3[36];  // Reserved for recipient (address/alias)
    unsigned char line4[45];
    unsigned char line5[45];
    unsigned char line6[22];
    unsigned char fee_amount[22];
    unsigned char tmp[22];
    bool pkhash;            // True if recipient is a Public Key Hash
    bool finished;          // True if the entire parsing process is complete
    cx_blake2b_t hash_ctx;  // Hash context for calculating transaction ID
    bool header_parsed;
} uiContext_t;

// A place to store data during the signing
typedef struct signingContext_t {
    union {
        uiContext_t ui;
        streamEddsaContext_t eddsa_context;
    };
    unsigned char sign_bit;
    unsigned char amount_decimals;
    unsigned char amount2_decimals;
    unsigned char fee_decimals;
    unsigned char data_type;
    unsigned char data_version;
    int message_type;
    unsigned char network_byte;
    unsigned char signature[64];
    unsigned char first_data_hash[45];
    uint8_t step;
    uint8_t sign_from;
    uint32_t bip32[5];
    uint32_t data_read;
    uint32_t data_size;
    uint32_t chunk_used;
    uint32_t chunk;
} signingContext_t;

// A place to store data during the confirming the address
typedef struct addressesContext_t {
    unsigned char address[36];
    unsigned char public_key[32];
} addressesContext_t;

typedef union {
    signingContext_t signing_context;
    addressesContext_t address_context;
} G_context_t;

/**
 * Structure for global context.
 */
typedef struct {
    state_e state;  /// state of the context
    signingContext_t signing_context;
    addressesContext_t address_context;
    request_type_e req_type;  /// user request
    uint32_t bip32_path[20];  /// BIP32 path
    uint8_t bip32_path_len;   /// length of BIP32 path
} global_ctx_t;

static const unsigned char WAVES_CONST[] = "Waves";