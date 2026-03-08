#include "sign.h"

// --- 1. Main process navigation (Review Flow) ---
void show_order_confirmation_flow(void) {
     uint8_t count = 0;

    // 1. (Amount)
    pairs[count].item = "Amount";
    pairs[count].value = (const char *)G_context.signing_context.ui.line4;
    count++;

    // 2. (asset)
    pairs[count].item = "Amount asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.line1;
    count++;

    // 3. (Matcher)
    pairs[count].item = "Matcher";
    pairs[count].value = (const char *)G_context.signing_context.ui.line2;
    count++;

    // 4. (Matcher Fee)
    pairs[count].item = "Matcher Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 5. (Fee asset)
    pairs[count].item = "Fee asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_asset;
    count++;

    // 6. (Fee)
    pairs[count].item = "Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 8. (From)
    pairs[count].item = "From";
    pairs[count].value = (const char *)G_context.signing_context.ui.from;
    count++;

    // 9. (ID)
    pairs[count].item = "Hash";
    pairs[count].value = (const char *)G_context.signing_context.first_data_hash;
    count++;

    // pair list
    pairList.nbPairs = count;
    pairList.pairs = pairs;
    pairList.nbMaxLinesForValue = 0;

    snprintf(review_title, sizeof(review_title), "Review Order");

    // Запуск процесса Review
    nbgl_useCaseReview(
        TYPE_TRANSACTION,           
        &pairList,                  
        &REVIEW_ICON,              
        review_title,             
        NULL,                      
        "Confirm Approve",          
        review_choice_callback
    );
}