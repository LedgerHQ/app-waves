/*******************************************************************************
 *   Waves Platform Wallet App for Nano Ledger devices
 *   Copyright (c) 2017-2020 Sergey Tolmachev (Tolsi) <tolsi.ru@gmail.com>
 *
 *   Based on Sample code provided (c) 2016 Ledger and
 *                                 (c) 2017-2018 Jake B. (Burstcoin app)
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 ********************************************************************************/

#include "ui_menus_buttons.h"

#include "../ui_logic.h"
#include "os_io_seproxyhal.h"

#if defined(TARGET_NANOS)
unsigned int ui_verify_transfer_nanos_button(unsigned int button_mask, unsigned int button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
            io_seproxyhal_cancel(NULL);
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:  // OK
            io_seproxyhal_touch_sign_approve(NULL);
            break;
    }
    return 0;
}

unsigned int ui_verify_transaction_nanos_button(unsigned int button_mask, unsigned int button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
            io_seproxyhal_cancel(NULL);
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:  // OK
            io_seproxyhal_touch_sign_approve(NULL);
            break;
    }
    return 0;
}

unsigned int ui_address_nanos_button(unsigned int button_mask, unsigned int button_mask_counter) {
    switch (button_mask) {
        case BUTTON_EVT_RELEASED | BUTTON_LEFT: // CANCEL
            io_seproxyhal_cancel(NULL);
            break;

        case BUTTON_EVT_RELEASED | BUTTON_RIGHT:  // OK
            io_seproxyhal_touch_address_ok(NULL);
            break;
        }
    return 0;
}
#endif