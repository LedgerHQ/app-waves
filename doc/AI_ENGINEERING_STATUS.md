# WAVES Ledger App — AI / engineering status

Summary of the **Waves Ledger Application (firmware 2.1.0, APDU spec revision 2.4, Modern SDK)** implementation and open work.

## Implemented

- **APDU (CLA `0xE0`)**
  - `0x06` **GET_APP_CONFIGURATION**: flags (`0x01` blind-sign capable), version triplet, app name length + ASCII `Waves`.
  - `0x04` **GET_PUBLIC_KEY**: P1 silent/confirm, P2 chain ID; data = 5-level BIP32 path; response = 32-byte Ed25519 pubkey + 1-byte address length + Base58 address.
  - `0x08` **SIGN_TX_STREAM**: P1 `0x00` INIT / `0x01` ADD / `0x80` LAST; P2 chain ID; streaming **SecureHash** = `Keccak256(Blake2b256(tx))` without buffering the full transaction; **0x6B00** if ADD/LAST without INIT; session ends streaming after LAST until user review (no extra chunks during UI).
  - **Utilities**: `0x03` GET_VERSION (3-byte triplet), `0xF4` GET_APP_NAME (raw ASCII name).
- **Cryptography**: SDK Ed25519 SLIP-10 (`HDW_ED25519_SLIP10`), `cx_eddsa_*_no_throw`, `bip32_derive_with_seed_*`; `explicit_bzero` on sensitive buffers and `G_context` where required.
- **Paths**: Makefile `44'/5741564'`, curve `ed25519`, app **Waves** / **2.1.x** (`APPVERSION`); shared path policy ([path_policy.c](../src/crypto/path_policy.c)) for GET_PUBLIC_KEY and SIGN.
- **UI (NBGL)**: Address review; blind-sign review (network line + transaction hash Base58).
- **Tests**: Ragger standalone tests; [corner-case APDU tests](../tests/standalone/test_sign_corner_cases.py); unit tests for INIT layout and path policy; fuzz `fuzz_sign_stream_init`.
- **Compliance docs**: [LEDGER_GUIDELINES_COMPLIANCE.md](LEDGER_GUIDELINES_COMPLIANCE.md), [EXTERNAL_AUDIT.md](EXTERNAL_AUDIT.md), [SECURITY.md](../SECURITY.md).
- **Removed**: Boilerplate transaction parser, tokens, Exchange/swap, dynamic TLV/PKI app paths, legacy `stream_eddsa_sign.c`.

## Follow-ups

- Regenerate **Ragger golden screenshots** per device after UI text changes.
- Align **Ledger Live / Waves Keeper** with new INS and streaming protocol; deprecate old [app-waves](https://github.com/vba2000/app-waves/tree/version-2.0.1) APDU where applicable.
- Independent **crypto / APDU** review; confirm Waves address and checksum vs production nodes.
- If the SDK build misses Blake2b or Keccak, add the correct **`HAVE_*`** flags for your `BOLOS_SDK`.
- Replace or refine **icons** if Ledger design review requests artwork tweaks (Waves-branded assets in `icons/app_waves_*`).

## Out of scope for device-app audit

- Host software (Waves Keeper, browser, full node).
- Ledger Live listing process (separate from this repo).
- Historical **Nano S** target: not listed in [ledger_app.toml](../ledger_app.toml); supported devices are Nano X, Nano S+, Stax, Flex, Apex P.

## References

- [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) — protocol definition.
- Bases: [Ledger app-boilerplate](https://github.com/LedgerHQ/app-boilerplate), legacy [app-waves](https://github.com/vba2000/app-waves/tree/version-2.0.1) (historical; protocol differs).
