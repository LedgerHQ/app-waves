#pragma once

#include "status_words.h"

/**
 * ADD/LAST without successful INIT (Waves spec 2.4, value 0x6B00).
 * Not named SWO_COMMAND_NOT_ALLOWED — SDK already defines that symbol with a different value.
 */
#define SWO_WAVES_SIGN_NO_INIT 0x6B00
