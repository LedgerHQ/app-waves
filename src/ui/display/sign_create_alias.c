#include "sign.h"


// --- 1.(Review Flow) ---
void show_create_alias_confirmation_flow(void) {
    uint8_t count = 0;

    // 1. (Amount)
    pairs[count].item = "Alias";
    pairs[count].value = (const char *)G_context.signing_context.ui.line1;
    count++;

    // 4. (Fee)
    pairs[count].item = "Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 6. (From)
    pairs[count].item = "From";
    pairs[count].value = (const char *)G_context.signing_context.ui.from;
    count++;

    // 7. (tx ID)
    pairs[count].item = "Transaction ID";
    pairs[count].value = (const char *)G_context.signing_context.first_data_hash;
    count++;

    // pair list
    pairList.nbPairs = count;
    pairList.pairs = pairs;
    pairList.nbMaxLinesForValue = 0;

    snprintf(review_title, sizeof(review_title), "Review Create Alias");

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