#include "sign.h"



// --- 1 (Review Flow) ---
void show_ivoke_confirmation_flow(void) {
    uint8_t count = 0;

    pairs[count].item = "dApp";
    pairs[count].value = (const char *)G_context.signing_context.ui.line3;
    count++;

    if (strlen((const char *)G_context.signing_context.ui.line2) > 0) {
        pairs[count].item = "Function";
        pairs[count].value = (const char *)G_context.signing_context.ui.line2;
        count++;
    }

    if (strlen((const char *)G_context.signing_context.ui.line4) > 0) {
        pairs[count].item = "Payment 1 amount";
        pairs[count].value = (const char *)G_context.signing_context.ui.line4;
        count++;
    }

    if (strlen((const char *)G_context.signing_context.ui.line1) > 0) {
        pairs[count].item = "Payment 1 asset";
        pairs[count].value = (const char *)G_context.signing_context.ui.line1;
        count++;
    }

    if (strlen((const char *)G_context.signing_context.ui.line6) > 0) {
        pairs[count].item = "Payment 2 amount";
        pairs[count].value = (const char *)G_context.signing_context.ui.line6;
        count++;
    }

    if (strlen((const char *)G_context.signing_context.ui.line5) > 0) {
        pairs[count].item = "Payment 2 asset";
        pairs[count].value = (const char *)G_context.signing_context.ui.line5;
        count++;
    }


    // 4.  (Fee)
    pairs[count].item = "Fee";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_amount;
    count++;

    // 5. (Fee Asset)
    pairs[count].item = "Fee asset";
    pairs[count].value = (const char *)G_context.signing_context.ui.fee_asset;
    count++;


    // 7. Отправитель (From)
    pairs[count].item = "From";
    pairs[count].value = (const char *)G_context.signing_context.ui.from;
    count++;

    // 8. ID транзакции (Hash)
    pairs[count].item = "Transaction ID";
    pairs[count].value = (const char *)G_context.signing_context.first_data_hash;
    count++;

    // Настройка списка пар
    pairList.nbPairs = count;
    pairList.pairs = pairs;
    pairList.nbMaxLinesForValue = 0; // Автоматическое определение высоты

    snprintf(review_title, sizeof(review_title), "Review Invoke");

    // Запуск процесса Review
    nbgl_useCaseReview(
        TYPE_TRANSACTION,           // Стиль: обзор транзакции
        &pairList,                  // Данные
        &REVIEW_ICON,                // Иконка глаза
        review_title,               // Заголовок первой страницы
        NULL,                       // Подзаголовок
        "Confirm Approve",          // Текст на кнопке подтверждения (Slide/Hold)
        review_choice_callback      // Колбэк
    );
}