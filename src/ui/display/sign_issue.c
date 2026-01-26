#include "sign.h"

// line 1 - name
// line 2 - description
// line 3 - amount
// line 5 - reissuable
// line 6 - has script

// --- 1.(Review Flow) ---
void show_issue_confirmation_flow(void) {
    uint8_t count = 0;

    // 1. (Amount)
    pairs[count].item = "Name";
    pairs[count].value = (const char *)G_context.signing_context.ui.line1;
    count++;

    // 2. (Description)
    pairs[count].item = "Description";
    pairs[count].value = (const char *)G_context.signing_context.ui.line2;
    count++;

    // 3. (Amount)
    pairs[count].item = "Amount";
    pairs[count].value = (const char *)G_context.signing_context.ui.line3;
    count++;

    // 4. (Reissuable)
    pairs[count].item = "Reissuable";
    pairs[count].value = (const char *)G_context.signing_context.ui.line5;
    count++;

    // 5. (Has script)
    pairs[count].item = "Has script";
    pairs[count].value = (const char *)G_context.signing_context.ui.line5;
    count++;

    // 6. (Fee)
    pairs[count].item = "Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 7. (Fee Asset)
    pairs[count].item = "Fee asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_asset;
    count++;


    // 8. (From)
    pairs[count].item = "From";
    pairs[count].value = (const char *)G_context.signing_context.ui.from;
    count++;

    // 9. (tx ID)
    pairs[count].item = "Transaction ID";
    pairs[count].value = (const char *)G_context.signing_context.first_data_hash;
    count++;

    // pair list
    pairList.nbPairs = count;
    pairList.pairs = pairs;
    pairList.nbMaxLinesForValue = 0;

    snprintf(review_title, sizeof(review_title), "Review Asset issue");

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