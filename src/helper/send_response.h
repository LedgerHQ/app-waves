#pragma once

#include "constants.h"
#include "os.h"

int helper_send_response_pubkey(void);

/** Send raw 64-byte Ed25519 signature. */
int helper_send_response_signature_ed25519(void);
