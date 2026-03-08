#include "show_sign_ui.h"
#include "globals.h"
#include "types.h"
#include "sign.h"

int show_sign_ui(void) {
    PRINTF("UI review type %02X \n", G_context.signing_context.data_type);
    bool is_proto = G_context.signing_context.message_type == PROTOBUF_DATA;
    
    switch (G_context.signing_context.data_type) {
        case 3:
            if (is_proto) {
                show_issue_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 4:
            show_transfer_confirmation_flow();
            return 0;     
        case 5:
            if (is_proto) {
                show_reissue_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 6:
            if (is_proto) {
                show_burn_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 8:
            if (is_proto) {
                show_lease_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 9:
            if (is_proto) {
                show_cancel_lease_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 10:
            if (is_proto) {
                show_create_alias_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 11:
            if (is_proto) {
                show_mass_transfer_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 12:
            if (is_proto) {
                show_data_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 13:
            if (is_proto) {
                show_set_script_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 14:
            if (is_proto) {
                show_sponsor_fee_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 15:
            if (is_proto) {
                show_set_asset_script_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 16:
            if (is_proto) {
                show_ivoke_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 17:
            if (is_proto) {
                show_update_asset_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 252:
            if (is_proto) {
                show_order_confirmation_flow();
                return 0;
            }
            show_legacy_tx_confirmation_flow(); 
            return 0;
        case 253:
        case 254:
        case 255:
            show_bytes_confirmation_flow();
            return 0;
    
        default:
            return SW_INCORRECT_TRANSACTION_TYPE_VERSION;
    }
}