# Standalone functional tests (WAVES)

Tests run with the **Waves** Ledger app opened from the dashboard (Speculos or a physical device).

## Stack

- [pytest](https://docs.pytest.org/)
- [Ragger](https://github.com/LedgerHQ/ragger)

## What is covered

- App launch and basic navigation
- `GET_VERSION` (0x03), `GET_APP_NAME` (0xF4), `GET_APP_CONFIGURATION` (0x06)
- `GET_PUBLIC_KEY` (0x04) with Waves BIP32 path `m/44'/5741564'/0'/0'/0'`
- `SIGN_TX_STREAM` (0x08) with Ed25519 signature check (PyNaCl)
- Error paths (bad CLA/INS, empty GET_PUBLIC_KEY, sign stream without INIT, etc.)
- **Corner cases** ([`test_sign_corner_cases.py`](test_sign_corner_cases.py)): `total=0`, over `MAX_TX_STREAM_TOTAL`, truncated INIT, wrong `chain_id` on ADD/LAST, chunk overflow, LAST with short count, second INIT overwriting session, invalid P1, LAST without INIT
- **Address encoding** ([`test_address_encoding.py`](test_address_encoding.py) offline; [`test_pubkey_cmd.py`](test_pubkey_cmd.py) on device): Base58 account address matches `waves_address_from_pubkey()` (same rules as `src/address.c` / legacy Waves address derivation)

## Layout

```text
standalone/
├── conftest.py
├── test_*.py
├── snapshots/
├── snapshots-tmp/          # local only, usually gitignored
├── requirements.txt
└── utils.py
```

## Run

From the repo root, after installing [requirements.txt](requirements.txt) and building the app binary for the target:

```shell
pytest tests/standalone/ --tb=short -v --device nanosp
```

More options: [usage.md](usage.md).

### Refreshing golden snapshots

After UI or test changes, rebuild the app for the same `--device`, then regenerate PNGs instead of comparing:

```shell
export PYTHONPATH=tests
pytest tests/standalone/ --tb=short -v --device nanosp --golden_run
```

Use a venv if `pip` reports an externally managed environment (see [doc/RUN_TESTS.md](../../doc/RUN_TESTS.md)). Repeat for each target (`nanox`, `stax`, …) with matching `BOLOS_SDK` and binary.

**CI:** In GitHub Actions, run workflow **Build and functional tests** manually with input **`golden_run: Open a PR`** so Ledger’s reusable workflow sets `regenerate_snapshots` and can open a PR with updated images (see [.github/workflows/build_and_functional_tests.yml](../../.github/workflows/build_and_functional_tests.yml)).
