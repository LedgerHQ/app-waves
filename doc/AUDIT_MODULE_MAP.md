# Application module map (security audit)

This document maps **WAVES Ledger App** sources to responsibilities (APDU details: [APP_SPECIFICATION.md](../APP_SPECIFICATION.md)). It complements [EXTERNAL_AUDIT.md](EXTERNAL_AUDIT.md): trust boundaries, reproducible build, and tests are described there.

**Out of scope for this repository:** host wallets, Ledger Live, Waves network nodes.

---

## 1. Entry point and global state

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/app_main.c](../src/app_main.c) | BOLOS loop: `io_init()`, NBGL main menu, `G_context` zeroing, APDU receive, command parse, dispatcher call. | All incoming APDUs go through this loop; parse-length errors return an SW without running signing business logic. |
| [src/globals.h](../src/globals.h) | `extern global_ctx_t G_context`. | Single session context; important for clearing secrets on state reset. |
| [src/types.h](../src/types.h) | UI and signing session enums; `global_ctx_t` (BIP32 path, `chain_id`, `pubkey` / `sign_stream` union), `waves_secure_hash_ctx_t` hashing context. | `STATE_SIGN_STREAM_UI` and `SIGN_SESSION_STREAMING` tie streaming signing to blind-sign UI. |
| [src/constants.h](../src/constants.h) | App constants: name, version, lengths, limits. | Consistency with Makefile (`APPNAME`) and `GET_VERSION` / `GET_APP_NAME` responses. |
| [src/sw.h](../src/sw.h) | Application status words (SW); does not override SDK values. | Cross-check with [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) and test expectations. |

---

## 2. APDU dispatcher

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/apdu/dispatcher.c](../src/apdu/dispatcher.c) | Validates `CLA`, routes by `INS`: `GET_VERSION`, `GET_APP_NAME`, `GET_APP_CONFIGURATION`, `GET_PUBLIC_KEY`, `SIGN_TX_STREAM`. Validates `P1`/`P2` and `buffer_t` for payload. | First line of control: wrong class/ins/parameters → SW without invoking handlers with data. |
| [src/apdu/dispatcher.h](../src/apdu/dispatcher.h) | Declares `apdu_dispatcher`. | — |

---

## 3. Command handlers

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/handler/get_version.c](../src/handler/get_version.c) | Returns version triplet. | Informational; no secrets. |
| [src/handler/get_app_name.c](../src/handler/get_app_name.c) | Returns app name from constants. | Consistency with Makefile / metadata. |
| [src/handler/get_app_configuration.c](../src/handler/get_app_configuration.c) | Capability flags (including blind-sign), version, name. | Affects host expectations; does not expose keys. |
| [src/handler/get_public_key.c](../src/handler/get_public_key.c) | SLIP-Ed25519 derivation, path policy, Waves address encoding; when confirming, prepares UI and waits for user approval. | **Critical:** path policy, buffer zeroing, address consistency with `src/address.c`. |
| [src/handler/sign_tx_stream.c](../src/handler/sign_tx_stream.c) | `INIT` / `ADD` / `LAST` stream: incremental SecureHash, total-length checks, blind-sign UI, Ed25519 signature after approval. | **Critical:** session state machine, reset on error, no signature without `validate_*`. |
| Adjacent `*.h` headers | INS-specific signatures and constants. | Aids static analysis navigation. |

---

## 4. Cryptography and path policy

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/crypto/secure_hash.c](../src/crypto/secure_hash.c) | **SecureHash** = `Keccak256(Blake2b256(payload))` over chunked input. | Hash correctness for signing; SDK API usage. |
| [src/crypto/secure_hash.h](../src/crypto/secure_hash.h) | `waves_secure_hash_ctx_t`, init/update/final API. | Boundary between byte stream and 32-byte signing hash. |
| [src/crypto/path_policy.c](../src/crypto/path_policy.c) | Only hardened segments allowed on Waves path `44'/5741564'/…`. | **Critical:** reduces risk from non-hardened path levels. |
| [src/crypto/path_policy.h](../src/crypto/path_policy.h) | Path check declarations. | — |

---

## 5. Address encoding

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/address.c](../src/address.c) | Builds Waves address string from pubkey and network byte (chain id). | Consistency with on-screen display and `GET_PUBLIC_KEY` responses; not the crypto core, but defines what the user sees as the address. |
| [src/address.h](../src/address.h) | Prototypes and length constants. | — |

---

## 6. User interface (NBGL) and user actions

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/ui/menu_nbgl.c](../src/ui/menu_nbgl.c) | Home and settings: `nbgl_useCaseHomeAndSettings` (no extra toggles in app settings). | App entry point; does not handle APDUs directly. |
| [src/ui/menu.h](../src/ui/menu.h) | Declares `ui_menu_main` / `ui_menu_about`. | — |
| [src/ui/display.h](../src/ui/display.h) | Declares signing / address screens. | Links UI to handlers. |
| [src/ui/nbgl_display_address.c](../src/ui/nbgl_display_address.c) | Address confirmation when `GET_PUBLIC_KEY` requests confirmation. | User must explicitly approve the displayed address. |
| [src/ui/nbgl_display_blind_sign.c](../src/ui/nbgl_display_blind_sign.c) | Blind-sign: network, tx hash in Base58, `nbgl_useCaseReviewBlindSigning`, approve/reject callback. | **Critical:** user sees hash and network, not raw transaction bytes. |
| [src/ui/action/validate.c](../src/ui/action/validate.c) | Completes flows: approve/reject address display, approve/reject blind-sign, sends SW/responses. | **Critical:** ties UI actions to actual signature or rejection. |
| [src/ui/action/validate.h](../src/ui/action/validate.h) | Declares `validate_*`. | — |

---

## 7. Response helpers

| File | Purpose | Audit notes |
|------|---------|---------------|
| [src/helper/send_response.c](../src/helper/send_response.c) | Unified APDU response sending (pointers, lengths). | Buffer bounds when building responses. |
| [src/helper/send_response.h](../src/helper/send_response.h) | Declarations. | — |

---

## 8. Suggested code reading order

1. **Data path:** `app_main.c` → `dispatcher.c` → relevant `handler/*.c`.
2. **Signing:** `sign_tx_stream.c` + `crypto/secure_hash.*` + `path_policy.c` + `validate.c` + `nbgl_display_blind_sign.c`.
3. **Address:** `get_public_key.c` + `address.c` + `nbgl_display_address.c` + `validate.c`.

---

## 9. Related artifacts

| Artifact | Location |
|----------|----------|
| Protocol and status codes | [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) |
| Build and tests | [doc/RUN_TESTS.md](RUN_TESTS.md), [README.md](../README.md) |
| Functional tests | [tests/standalone/](../tests/standalone/) |
| Unit tests | [unit-tests/](../unit-tests/) |
| Fuzzing | [fuzzing/README.md](../fuzzing/README.md) |
| Vulnerability disclosure | [SECURITY.md](../SECURITY.md) |

---

*This document reflects the repository layout at the time of writing; update it when adding or reorganizing files under `src/`.*
