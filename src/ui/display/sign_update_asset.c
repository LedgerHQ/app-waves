#include "sign.h"

// line 1 - Asset
// line 2 - name
// line 4 - description


// --- 1.(Review Flow) ---
void show_update_asset_confirmation_flow(void) {
    uint8_t count = 0;

    // 1. (Amount)
    pairs[count].item = "Asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.line1;
    count++;

    // 2. (Description)
    pairs[count].item = "Name";
    pairs[count].value = (const char *)G_context.signing_context.ui.line2;
    count++;

    // 3. (Amount)
    pairs[count].item = "Description";
    pairs[count].value = (const char *)G_context.signing_context.ui.line4;
    count++;

    // 4. (Fee)
    pairs[count].item = "Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 5. (Fee Asset)
    pairs[count].item = "Fee asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_asset;
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

    snprintf(review_title, sizeof(review_title), "Review Update Asset");

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