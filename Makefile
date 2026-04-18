# ****************************************************************************
#    WAVES Ledger Application
#    Copyright (c) 2026 Waves Ledger App contributors.
#
#    Based on Ledger App Boilerplate (c) 2023 Ledger SAS.
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
# ****************************************************************************

ifeq ($(BOLOS_SDK),)
$(error Environment variable BOLOS_SDK is not set)
endif

include $(BOLOS_SDK)/Makefile.target

########################################
#        Mandatory configuration       #
########################################
APPNAME = "Waves"

APPVERSION_M = 2
APPVERSION_N = 1
APPVERSION_P = 0
APPVERSION = "$(APPVERSION_M).$(APPVERSION_N).$(APPVERSION_P)"

APP_SOURCE_PATH += src

ICON_NANOX = icons/app_waves_14px.gif
ICON_NANOSP = icons/app_waves_14px.gif
ICON_STAX = icons/app_waves_32px.gif
ICON_FLEX = icons/app_waves_40px.gif
ICON_APEX_P = icons/app_waves_32px_apex.png

ifeq ($(TARGET_NAME),$(filter $(TARGET_NAME),TARGET_NANOX TARGET_NANOS2))
    ICON_HOME_NANO = icons/app_waves_14px.gif
endif

CURVE_APP_LOAD_PARAMS = ed25519

# SLIP-44 coin type 5741564' (WAVES) — hardened
PATH_APP_LOAD_PARAMS = "44'/5741564'"

VARIANT_PARAM = COIN
VARIANT_VALUES = WAVES

#DEBUG = 1

ENABLE_BLUETOOTH = 1
ENABLE_NBGL_FOR_NANO_DEVICES = 1
ENABLE_NBGL_QRCODE = 1

# Waves app: no Exchange swap, dynamic tokens, or PKI
# ENABLE_SWAP / ENABLE_TLV_LIBRARY / ENABLE_TESTING_SWAP disabled

include $(BOLOS_SDK)/Makefile.standard_app
