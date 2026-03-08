
#include "sign.h"

// --- 1. Main process navigation (Review Flow) ---
void show_legacy_tx_confirmation_flow(void) {
    // prepare data for review
    uint8_t count = 0;

    pairs[count].item = "From";
    pairs[count].value = (const char *)G_context.signing_context.ui.from;
    count++;

    if (strlen((const char *)G_context.signing_context.ui.fee_amount) > 0) {
        pairs[count].item = "Fee";
        pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
        count++;
    }

    if (strlen((const char *)G_context.signing_context.ui.fee_asset) > 0) {
        pairs[count].item = "Fee asset";
        pairs[count].value = (const char *)G_context.signing_context.ui.fee_asset;
        count++;
    }

    pairs[count].item = "Transaction ID";
    pairs[count].value = (const char *)G_context.signing_context.first_data_hash;
    count++;

    pairList.nbPairs = count;
    pairList.pairs = pairs;
    pairList.nbMaxLinesForValue = 0;
    snprintf(review_title, sizeof(review_title), "Review %s", 
             (char *)G_context.signing_context.ui.line1);
    // Start Review process:
    // List of items to review + callback function
    nbgl_useCaseReview(
        TYPE_TRANSACTION,           // Style: transaction review
        &pairList,                  // Data to review
        &REVIEW_ICON,             // Icon "review"
        review_title,         // First page title
        NULL,                       // Subtitle
        "Confirm Approve",          // Last button text (Hold to confirm)
        review_choice_legacy_callback      // Callback function
    );
}

