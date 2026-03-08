#pragma once

#include <stdbool.h>  // bool


#if defined(TARGET_NANOX) || defined(TARGET_NANOS2)
    #define ICON_APP_WAVES C_app_waves_14px
    #define ICON_APP_HOME        C_home_waves_14px
    #define ICON_APP_WARNING     C_icon_warning
#elif defined(TARGET_STAX) || defined(TARGET_FLEX)
    #define ICON_APP_WAVES C_app_waves_64px
    #define ICON_APP_HOME        ICON_APP_WAVES
    #define ICON_APP_WARNING     C_Warning_64px
    #define REVIEW_ICON ICON_APP_HOME
#elif defined(TARGET_APEX_P)
    #define ICON_APP_WAVES C_app_waves_48px
    #define ICON_APP_HOME        ICON_APP_WAVES
    #define ICON_APP_WARNING     LARGE_WARNING_ICON
    #define REVIEW_ICON ICON_APP_HOME
#endif

/**
 * Callback to reuse action with approve/reject in step FLOW.
 */
typedef void (*action_validate_cb)(bool);

/**
 * Display address on the device and ask confirmation to export.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_address();

/**
 * Display transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_transaction(void);

/**
 * Display blind-sign transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_blind_signed_transaction(void);

/**
 * Display token transaction information on the device and ask confirmation to sign.
 *
 * @return 0 if success, negative integer otherwise.
 *
 */
int ui_display_token_transaction(void);
