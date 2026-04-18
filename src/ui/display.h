#pragma once

#include <stdbool.h>

#if defined(TARGET_NANOX) || defined(TARGET_NANOS2)
#define ICON_APP_WAVES C_app_waves_14px
#define ICON_APP_HOME  C_app_waves_14px
#define ICON_APP_WARNING C_icon_warning
#elif defined(TARGET_STAX)
#define ICON_APP_WAVES C_app_waves_32px
#define ICON_APP_HOME  ICON_APP_WAVES
#define ICON_APP_WARNING C_Warning_64px
#elif defined(TARGET_FLEX)
#define ICON_APP_WAVES C_app_waves_40px
#define ICON_APP_HOME  ICON_APP_WAVES
#define ICON_APP_WARNING C_Warning_64px
#elif defined(TARGET_APEX_P)
#define ICON_APP_WAVES C_app_waves_32px_apex
#define ICON_APP_HOME  ICON_APP_WAVES
#define ICON_APP_WARNING LARGE_WARNING_ICON
#endif

int ui_display_address(void);

/** Blind signing review: network + SecureHash (Base58). */
int ui_display_blind_signing(void);
