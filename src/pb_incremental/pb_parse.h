#ifndef PB_PARSE_H
#define PB_PARSE_H

#include "bs58.h"
#include <stdint.h>
#include <stdbool.h>
#include "os.h"
#include "pb_decode.h"
#include "transaction.pb.h"
#include "print_amount.h"
#include "buffer_helper.h"
#include "../../crypto/waves.h"
#include "../globals.h"
#include "cx.h"

bool buffer_peek_varint(const uint8_t *ptr, size_t len, uint64_t *value, size_t *v_len);
bool pb_incremental_parse(uiProtobuf_t *ctx, buffer_t *chunk, uint32_t total_size);
void pb_process_layer1_field(uiProtobuf_t *ctx, uint32_t tag, uint8_t *data, size_t len);
void pb_process_layer2_field(uiProtobuf_t *ctx, uint8_t b);
bool build_protobuf_root_tx_incremental(uiContext_t *ctx, buffer_t *chunk, uint32_t data_size);
bool pb_incremental_parse_order(uiContext_t *ctx, buffer_t *chunk, uint32_t total_size);

bool parse_issue_data_incremental(uiContext_t *ctx);
bool parse_transfer_data_incremental(uiContext_t *ctx);
bool parse_reissue_data_incremental(uiContext_t *ctx);
bool parse_burn_data_incremental(uiContext_t *ctx);
bool parse_lease_data_incremental(uiContext_t *ctx);
bool parse_lease_cancel_data_incremental(uiContext_t *ctx);
bool parse_create_alias_data_incremental(uiContext_t *ctx);
bool parse_mass_transfer_data_incremental(uiContext_t *ctx);
bool parse_data_data_incremental(uiContext_t *ctx);
bool parse_sponsor_fee_data_incremental(uiContext_t *ctx);
bool parse_set_asset_script_data_incremental(uiContext_t *ctx);
bool parse_invoke_script_data_incremental(uiContext_t *ctx);
bool parse_update_asset_info_data_incremental(uiContext_t *ctx);

int get_waves_transaction_header(uint32_t tag);
int get_waves_transaction_type(uint32_t tag);
void copy_string_with_dots(char *dest, uint8_t *src, size_t src_len, size_t max_chars);
bool asset_callback_incremental(pb_istream_t *stream, const pb_field_iter_t *field, void **arg);

#endif