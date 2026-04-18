# Changelog

All notable changes to this project are documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and versioning follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [2.1.0] - 2026-04

**Firmware version 2.1.0** (`Makefile` `APPVERSION_*`) for Ledger distribution; **APDU protocol specification** remains **revision 2.4** ([APP_SPECIFICATION.md](APP_SPECIFICATION.md)).

### Added

- Step-by-step test guide: [doc/RUN_TESTS.md](doc/RUN_TESTS.md) (Docker, unit tests, Ragger, CI parity).
- Audit-oriented documentation: [SECURITY.md](SECURITY.md), [doc/EXTERNAL_AUDIT.md](doc/EXTERNAL_AUDIT.md), [doc/LEDGER_GUIDELINES_COMPLIANCE.md](doc/LEDGER_GUIDELINES_COMPLIANCE.md), [doc/AUDIT_MODULE_MAP.md](doc/AUDIT_MODULE_MAP.md), [doc/SECURITY_REVIEW_NOTES.md](doc/SECURITY_REVIEW_NOTES.md), [doc/LEDGER_SUBMISSION.md](doc/LEDGER_SUBMISSION.md).
- WAVES Ledger Application on Modern SDK ([app-boilerplate](https://github.com/LedgerHQ/app-boilerplate) base).
- APDU spec 2.4: `GET_APP_CONFIGURATION` (0x06), `GET_PUBLIC_KEY` (0x04) with P2 chain ID, `SIGN_TX_STREAM` (0x08) with INIT/ADD/LAST.
- Utility commands: `GET_VERSION` (0x03), `GET_APP_NAME` (0xF4).
- Streaming **SecureHash** (`Keccak256(Blake2b256)`) without buffering the full transaction.
- Ed25519 signing with **SLIP-0010** and path `m/44'/5741564'/account'/0'/address_index'`.
- NBGL flows: address confirmation, blind-sign review (network + hash in Base58).
- Ragger tests and Python client updated for Waves; unit tests (INIT payload layout, BIP32 path policy); standalone corner-case APDU tests; fuzz target `fuzz_sign_stream_init`.
- Standalone tests: `GET_PUBLIC_KEY` address string checked against Python `waves_address_from_pubkey()` (matches `src/address.c`); offline reference tests in `test_address_encoding.py`.

### Removed

- Unused `app_boilerplate_*` files under `icons/` and `glyphs/` (the build uses only `icons/app_waves_*`; `glyphs/` may stay empty in-tree while SDK generates home glyphs under `build/`).
- Boilerplate transaction parser, token database, Exchange/swap integration, CAL/dynamic token APDUs.
- Legacy `stream_eddsa_sign` / secp256k1 boilerplate paths.

### Fixed

- Build and unit tests with current `ledger-secure-sdk`: `#include <stdbool.h>` in [src/address.h](src/address.h); `cx_eddsa_get_public_key_no_throw` extra parameters and **Ed25519 public key layout** (65-byte uncompressed `W` + `cx_edwards_compress_point_no_throw` → 32-byte canonical key) in [src/handler/get_public_key.c](src/handler/get_public_key.c); unit test [unit-tests/CMakeLists.txt](unit-tests/CMakeLists.txt) links `varint.c`; [unit-tests/test_path_policy.c](unit-tests/test_path_policy.c) includes headers required before `<cmocka.h>`.
- Status word `0x6B00` renamed to **`SWO_WAVES_SIGN_NO_INIT`** in [src/sw.h](src/sw.h) (avoids redefining SDK `SWO_COMMAND_NOT_ALLOWED`).
- **GET_PUBLIC_KEY** response: zero address buffer before [waves_encode_address](src/address.c); length via `strnlen`; Python [unpack](tests/application_client/boilerplate_response_unpacker.py) strips whitespace.
- Signing stream uses **`waves_secure_hash_ctx_t`** in [src/types.h](src/types.h); [src/crypto/secure_hash.c](src/crypto/secure_hash.c) adds explicit `cx_blake2b_*` declarations for clean builds.

### Tests / snapshots

- Ragger **golden PNGs** under [tests/standalone/snapshots/](tests/standalone/snapshots/) refreshed for **nanosp**, **nanox**, **stax**, **flex**, and **apex_p** (CI matrix); removed leftover boilerplate snapshot directories (token/legacy tx names).
- [tests/standalone/test_sign_cmd.py](tests/standalone/test_sign_cmd.py) `test_sign_tx_stream_short` remains **xfail** (non-strict) when Speculos returns `ChunkedEncodingError` during blind-sign async polling on some setups.
- Touch devices: CI runs the same Ragger matrix as Nano; blind-sign **refuse** path uses `review_reject_with_warning` (aligns with blind warning + `Hold to sign`); main menu uses **settings open + exit** for empty NBGL settings (no dummy switches); main menu uses a longer navigation timeout on non-Nano devices.

### Changed

- Makefile: `APPNAME=Waves`, curve `ed25519`, derivation path `44'/5741564'`.
- [.github/workflows/build_and_functional_tests.yml](.github/workflows/build_and_functional_tests.yml): `run_for_devices` includes `nanosp`, `nanox`, `stax`, `flex`, `apex_p`.
- [tests/standalone/requirements.txt](tests/standalone/requirements.txt): upper bounds on major versions for more reproducible Ragger/Speculos runs.
- [doc/TRANSACTION.md](doc/TRANSACTION.md): blind-signing threat model for integrators; [doc/SECURITY_REVIEW_NOTES.md](doc/SECURITY_REVIEW_NOTES.md) formalizes review scope and residual risks.
- Documentation rewritten for Waves (`README`, `APP_SPECIFICATION`, tests docs, fuzzing README).

---

## Historical boilerplate releases (fork ancestry)

The following entries describe the upstream **Ledger app-boilerplate** history before this repository was repurposed as WAVES.

### [2.1.0] - 2023-10-06 (boilerplate)

- Settings / NVM-related NBGL improvements.

### [2.0.0] - 2023-07-10 (boilerplate)

- Stax porting, CI, Ragger tests, Makefile.standard_app migration, NBGL updates, `_no_throw` crypto.

### [1.0.0] - 2020-11-19 (boilerplate)

- Initial boilerplate application.
