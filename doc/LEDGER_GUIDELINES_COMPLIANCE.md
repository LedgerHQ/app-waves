# Ledger guidelines compliance (embedded app)

This document records how the WAVES application aligns with Ledger requirements and what to verify before submitting to [Ledger Live / the app catalog](https://developers.ledger.com/).

## Automated checks (CI)

The repository uses the **`guidelines_enforcer`** workflow ([`.github/workflows/guidelines_enforcer.yml`](../.github/workflows/guidelines_enforcer.yml)), which calls [`LedgerHQ/ledger-app-workflows`](https://github.com/LedgerHQ/ledger-app-workflows) `reusable_guidelines_enforcer.yml@v1`. Typical checks include:

| Check | Meaning |
|-------|---------|
| **README** | Non-empty `README.md`; Markdown headings (`# ...`) must not use “Boilerplate” as the app name. |
| **APP_SPECIFICATION.md** | If present, headings must not use “Boilerplate” as the spec title. |
| **Icons** | Icon files from the build manifest: geometry per device (e.g. 14×14 Nano, 32×32 Stax), no boilerplate icons (name/`boilerplate`, known MD5s). See [Design requirements](https://developers.ledger.com/docs/embedded-app/design-requirements/). |
| **Makefile** | `APPNAME` / `VARIANT` not `boilerplate`; no legacy `HAVE_BOLOS_UX`; allowed standard Makefile; dangerous flags blocked in dry-run build. |
| **App load params** | Load parameters consistent with the manifest. |
| **Clang static analyzer** | Static analysis of C code. |

CI also typically runs: `coding_style_checks` (clang-format via [`../.clang-format`](../.clang-format)), build, unit tests, Ragger, Doxygen, and optionally CodeQL/ClusterFuzzLite.

## Application manifest

[`ledger_app.toml`](../ledger_app.toml) defines:

- `build_directory`, `sdk = "C"`, and the `devices` list;
- `[use_cases]` (e.g. `debug = "DEBUG=1"`);
- `[unit_tests]` → `./unit-tests/`;
- `[pytest.standalone]` → `./tests/standalone/`.

The Ledger VS Code extension and `ledger-manifest` use the same file when validating the Makefile.

## Current repository state (manual checklist)

| Requirement | WAVES app |
|-------------|-----------|
| App name in Makefile | `APPNAME = "Waves"` |
| Variant | `VARIANT_VALUES = WAVES` |
| Curve / path | `ed25519`, `44'/5741564'` |
| Icons | `icons/app_waves_*.gif` / `.png` (not `*boilerplate*`) |
| No swap / legacy UX | Exchange swap disabled; `HAVE_BOLOS_UX` absent |
| Specification | [`APP_SPECIFICATION.md`](../APP_SPECIFICATION.md) describes protocol 2.4 |
| Tests | `tests/standalone/`, `unit-tests/` |

## Running checks locally

1. **Build** in Docker [`ledger-app-dev-tools`](https://github.com/LedgerHQ/ledger-app-builder/pkgs/container/ledger-app-builder%2Fledger-app-dev-tools) with `BOLOS_SDK` set for the target device (`make`).
2. **Formatting**: `clang-format -i $(find src -name '*.[ch]')` (or what the reusable lint runs).
3. **Icons**: the CI image usually includes ImageMagick `identify`; locally you can verify geometry and alpha per the [design guide](https://developers.ledger.com/docs/embedded-app/design-requirements/).
4. **Full guidelines suite** — easiest on a **fork**: enable GitHub Actions and wait for green `guidelines_enforcer`, or clone `ledger-app-workflows` and run `scripts/check_all.sh` (as in CI) for a full local equivalent.

## External audit

Reviewer package: [EXTERNAL_AUDIT.md](EXTERNAL_AUDIT.md), [SECURITY_REVIEW_NOTES.md](SECURITY_REVIEW_NOTES.md) (threat model and review summary), [SECURITY.md](../SECURITY.md).

## Additional Ledger resources

- [Embedded app: structure & guidelines](https://developers.ledger.com/docs/embedded-app/)  
- [Publishing a Ledger application](https://developers.ledger.com/docs/embedded-app/publish-app/)  
- Submission checklist (this repo): [LEDGER_SUBMISSION.md](LEDGER_SUBMISSION.md)
