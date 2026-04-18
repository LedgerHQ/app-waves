/*
 * WAVES Ledger Application.
 * Copyright (c) 2026 Waves Ledger App contributors.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "os.h"
#include "glyphs.h"
#include "nbgl_use_case.h"

#include "menu.h"
#include "display.h"
#include "constants.h"

static void app_quit(void) {
    os_sched_exit(-1);
}

#define SETTING_INFO_NB 1
static const char *const INFO_TYPES[SETTING_INFO_NB] = {"Version"};
static const char *const INFO_CONTENTS[SETTING_INFO_NB] = {APPVERSION};

static const nbgl_contentInfoList_t infoList = {
    .nbInfos = SETTING_INFO_NB,
    .infoTypes = INFO_TYPES,
    .infoContents = INFO_CONTENTS,
};

static const nbgl_genericContents_t emptySettings = {
    .callbackCallNeeded = false,
    .contentsList = NULL,
    .nbContents = 0,
};

void ui_menu_main(void) {
    nbgl_useCaseHomeAndSettings(APPNAME,
                                &ICON_APP_HOME,
                                NULL,
                                INIT_HOME_PAGE,
                                &emptySettings,
                                &infoList,
                                NULL,
                                app_quit);
}

void ui_menu_about(void) {
    ui_menu_main();
}
