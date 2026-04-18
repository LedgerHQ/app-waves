# Tests

## Layout

- **`application_client/`** — Minimal Python helpers to build APDUs and parse responses for pytest (not a full production SDK).
- **`standalone/`** — Functional tests with [Ragger](https://github.com/LedgerHQ/ragger): app launched from the device dashboard (Speculos or hardware).

The **Exchange / swap** test suite was removed with the WAVES app; there is no `os_lib_call` swap flow in this project.

The directory `tests/swap/` (if present) is **legacy boilerplate only** and is **not** run by CI; use `tests/standalone/` for functional tests.

See [standalone/README.md](standalone/README.md) for running the main test suite.
