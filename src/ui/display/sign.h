#include <string.h>
#include "nbgl_use_case.h"
#include "ux.h"
#include "os.h"
#include "globals.h"
#include "send_response.h"
#include "menu.h"
#include "glyphs.h"
#include "display.h"


void show_transfer_confirmation_flow(void);  
void show_burn_confirmation_flow(void);
void show_cancel_lease_confirmation_flow(void);
void show_create_alias_confirmation_flow(void);
void show_data_confirmation_flow(void);  
void show_bytes_confirmation_flow(void);
void show_issue_confirmation_flow(void);
void show_lease_confirmation_flow(void);
void show_legacy_tx_confirmation_flow(void);
void show_mass_transfer_confirmation_flow(void);
void show_reissue_confirmation_flow(void);
void show_set_asset_script_confirmation_flow(void);
void show_set_script_confirmation_flow(void);
void show_sponsor_fee_confirmation_flow(void);
void show_transfer_confirmation_flow(void);
void show_ivoke_confirmation_flow(void);
void show_update_asset_confirmation_flow(void);
void show_order_confirmation_flow(void);



#ifndef UI_SIGN_H
#define UI_SIGN_H

static nbgl_layoutTagValue_t pairs[10];
static nbgl_layoutTagValueList_t pairList;
static char review_title[64];
static char result_message[64];

// --- Function for starting spinner at start ---
__attribute__((unused))
static void wait_data_spinner(void) {

   nbgl_useCaseSpinner("Wait data...");
}

__attribute__((unused))
static void status_back_to_idle(void) {
    ui_menu_main(); 
}

__attribute__((unused))
static void review_choice_legacy_callback(bool confirm) {
    if (confirm) {
        // Show success message and return to menu after 2 seconds
        snprintf(result_message, sizeof(result_message), "%s Signed", 
             (char *)G_context.signing_context.ui.line1);
        set_result_sign();
        explicit_bzero(&G_context, sizeof(G_context));
        nbgl_useCaseStatus(result_message, true, status_back_to_idle);
    } else {
        io_send_sw(SW_DENY);
        // Show rejection message
        explicit_bzero(&G_context, sizeof(G_context));
        nbgl_useCaseStatus("Transaction Rejected", false, status_back_to_idle);
    }
}

__attribute__((unused))
static void review_choice_callback(bool confirm) {
    if (confirm) {
        set_result_sign();
        
        snprintf(result_message, sizeof(result_message), "Transaction Signed");
        explicit_bzero(&G_context, sizeof(G_context));
        nbgl_useCaseStatus(result_message, true, status_back_to_idle);
    } else {
        explicit_bzero(&G_context, sizeof(G_context));
        io_send_sw(SW_DENY);
        nbgl_useCaseStatus("Transaction Rejected", false, status_back_to_idle);
    }
    
}
#endif

