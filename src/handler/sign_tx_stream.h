#pragma once

#include <stdint.h>

#include "buffer.h"

/** SIGN_TX_STREAM (INS 0x08). @p chain_id from APDU P2. */
int handler_sign_tx_stream(buffer_t *cdata, uint8_t p1, uint8_t chain_id);
