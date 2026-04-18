# Pull request checklist

- [ ] Change set matches project scope (WAVES Ledger app, spec 2.4).
- [ ] `APP_SPECIFICATION.md` / tests updated if APDU or behavior changed.
- [ ] Application version bumped in `Makefile` if a release is intended.
- [ ] CI-style checks considered: `clang-format`, build, `pytest tests/standalone/`, `unit-tests` (with `BOLOS_SDK`).

For Ledger App Store submissions, follow [Ledger’s maintenance and deliver process](https://developers.ledger.com/docs/device-app/deliver/maintenance).
