### Summary

This PR delivers a **full rewrite** of the WAVES Ledger device application for the **current Ledger stack** (`ledger-secure-sdk`, **NBGL**), aligned with **Ledger device-app guidelines** and the reusable CI workflows used across Ledger apps.

It **replaces** the legacy on-device design (older APDU layout, older UX stack). Earlier community work and forks targeted that legacy line; this tree is intentionally a **clean replacement**, not a small patch on the old codebase.

### What changed (high level)

- **Simpler firmware model:** hash-only signing path (no full-transaction buffer in RAM); streaming **SecureHash** = `Keccak256(Blake2b256(tx bytes))`, **Ed25519** (SLIP-0010), blind-sign review on device where applicable.
- **Protocol:** **APDU specification revision 2.4** — see `APP_SPECIFICATION.md` (`SIGN_TX_STREAM`, `GET_PUBLIC_KEY` with chain id, `GET_APP_CONFIGURATION`, etc.).
- **Firmware version:** **2.1.0** (`Makefile` / `GET_VERSION`).
- **Devices:** Nano X, Nano S+, Stax, Flex, Apex P — see `ledger_app.toml` and CI `run_for_devices`.
- **Tests:** Ragger functional suite under `tests/standalone/`; unit tests under `unit-tests/`.

### Relation to previous PRs and forks

- **[PR #17](https://github.com/LedgerHQ/app-waves/pull/17)** and similar PRs refer to the **legacy** app line (different protocol and implementation). This PR **supersedes** that direction for a **modern SDK** submission.
- If Ledger prefers to **close** outdated open PRs in favor of this work, please reference them here (e.g. #17).

### Documentation

| Topic | Location |
| --- | --- |
| APDU protocol | `APP_SPECIFICATION.md` |
| How to run tests | `doc/RUN_TESTS.md` |
| Security / audit handoff | `doc/EXTERNAL_AUDIT.md`, `doc/SECURITY_REVIEW_NOTES.md`, `doc/AUDIT_MODULE_MAP.md` |
| Ledger checklist | `doc/LEDGER_SUBMISSION.md`, `doc/LEDGER_GUIDELINES_COMPLIANCE.md` |
| Fork → PR workflow | `doc/FORK_AND_PR.md` |

### Testing

- **CI (fork):** `guidelines_enforcer`, `build_and_functional_tests` (build + Ragger for `nanosp`, `nanox`, `stax`, `flex`, `apex_p`), `unit_tests`, `coding_style_checks`, `python_client_checks`, and other workflows enabled on the repository.
- **Local / Docker:** `ledger-app-dev-tools` — `make` per target, then `pytest tests/standalone/ --device <target>` per `doc/RUN_TESTS.md`.

### Checklist

- [ ] Icons and metadata meet [Ledger design requirements](https://developers.ledger.com/docs/embedded-app/design-requirements/)
- [ ] `CHANGELOG.md` updated for this release
- [ ] Green CI on the submitted commit (all required workflows)
