#include "sign.h"


// --- 1. Main process navigation (Review Flow) ---
void show_bytes_confirmation_flow(void) {
    // prepare data for review
    pairs[0].item = "Hash";
    pairs[0].value = (const char *)G_context.signing_context.first_data_hash;

    pairs[1].item = "From";
    pairs[1].value = (const char *)G_context.signing_context.ui.from;

    pairList.nbPairs = 2;
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
