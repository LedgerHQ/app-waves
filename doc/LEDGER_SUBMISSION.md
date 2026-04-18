# Ledger submission (checklist)

Use this checklist when opening a **release** or **app update request** to Ledger (see [Publishing a Ledger application](https://developers.ledger.com/docs/embedded-app/publish-app/) for the current process).

## Versioning

- **Firmware / `APPVERSION`:** defined in [`Makefile`](../Makefile) (`APPVERSION_M/N/P`) and defaults in [`src/constants.h`](../src/constants.h). Must match `GET_VERSION` (INS `0x03`) and [`GET_APP_CONFIGURATION`](../APP_SPECIFICATION.md).
- **APDU protocol:** documented as **specification revision 2.4** in [`APP_SPECIFICATION.md`](../APP_SPECIFICATION.md) — independent from the semver triplet above.

## Pre-submit (repository)

1. Green CI on the release commit: `guidelines_enforcer`, `build_and_functional_tests`, `unit_tests`, `coding_style_checks`, `python_client_checks` (and any other workflows you enable).
2. [`CHANGELOG.md`](../CHANGELOG.md) updated for the release.
3. Icons and metadata per [design requirements](https://developers.ledger.com/docs/embedded-app/design-requirements/) — see [`doc/LEDGER_GUIDELINES_COMPLIANCE.md`](LEDGER_GUIDELINES_COMPLIANCE.md).
4. Optional evidence pack: [`doc/EXTERNAL_AUDIT.md`](EXTERNAL_AUDIT.md), [`doc/SECURITY_REVIEW_NOTES.md`](SECURITY_REVIEW_NOTES.md), [`doc/AUDIT_MODULE_MAP.md`](AUDIT_MODULE_MAP.md).

## Pull request to LedgerHQ/app-waves

If the canonical upstream for the Waves app is **[LedgerHQ/app-waves](https://github.com/LedgerHQ/app-waves)** (`develop`): **fork → replace tree → PR** — [FORK_AND_PR.md](FORK_AND_PR.md). PR title/body (English): [UPSTREAM_PR_APP_WAVES.md](UPSTREAM_PR_APP_WAVES.md), body file [UPSTREAM_PR_BODY.md](UPSTREAM_PR_BODY.md) (e.g. vs [#17](https://github.com/LedgerHQ/app-waves/pull/17)).

## What to send Ledger

- Repository URL and **tag** or release name for the submitted build (e.g. `v2.1.0`).
- Short **release notes** (can mirror the changelog section for that version).
- Follow Ledger’s **current** submission channel (developer portal, GitHub PR to an app list, or email — process may change; use their official docs first).

## After submission

- Track Ledger review feedback; refresh golden Ragger snapshots if UI copy changes.
- Host integration (Ledger Live / Waves Keeper) is outside this firmware repo — see [`AI_ENGINEERING_STATUS.md`](AI_ENGINEERING_STATUS.md).
