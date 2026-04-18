# WAVES Ledger Application

Ledger device application for the [Waves](https://waves.tech/) blockchain. **Firmware version** is defined in `Makefile` (`APPVERSION_*`, currently **2.1.0**). It implements **protocol spec 2.4**: streaming blind signing with **SecureHash** (`Keccak256(Blake2b256)`), **Ed25519** (SLIP-0010), and the APDU layout described in [APP_SPECIFICATION.md](APP_SPECIFICATION.md).

The codebase is derived from [Ledger app-boilerplate](https://github.com/LedgerHQ/app-boilerplate) and replaces the boilerplate transaction parser with a hash-only signer (no full-transaction buffer in RAM).

## Quick start

### VS Code

Use [Ledger’s VS Code extension](https://marketplace.visualstudio.com/items?itemName=LedgerHQ.ledger-dev-tools) and the [ledger-app-dev-tools](https://github.com/LedgerHQ/ledger-app-builder/pkgs/container/ledger-app-builder%2Fledger-app-dev-tools) image to build, run on Speculos, run tests, and load the app.

- Install Docker and an X server (Linux: usually default; macOS: [XQuartz](https://www.xquartz.org/); Windows: [VcXsrv](https://sourceforge.net/projects/vcxsrv/)).
- Open this repository in VS Code with the Ledger extension.
- Use **Build**, **Run with Speculos**, **Run tests**, etc. (or `Ctrl+Shift+B` / `Cmd+Shift+B`).

The extension log in the terminal shows the underlying commands.

### Shell and Docker

Pull the image:

```shell
sudo docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

Enter the environment from the repository root:

**Linux**

```shell
sudo docker run --rm -ti --user "$(id -u):$(id -g)" --privileged -v "/dev/bus/usb:/dev/bus/usb" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

**macOS**

```shell
sudo docker run --rm -ti --user "$(id -u):$(id -g)" --privileged -v "$(pwd -P):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

**Windows (PowerShell)**

```shell
docker run --rm -ti --privileged -v "$(Get-Location):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-dev-tools:latest
```

### Build

Inside the container:

```shell
make DEBUG=1
```

Set `BOLOS_SDK` for the target (Nano X, Nano S+, Stax, Flex, Apex P), for example:

- `BOLOS_SDK=$NANOX_SDK`
- `BOLOS_SDK=$NANOSP_SDK`
- `BOLOS_SDK=$STAX_SDK`
- `BOLOS_SDK=$FLEX_SDK`
- `BOLOS_SDK=$APEX_SDK`

Default in the image is typically Nano S+.

### Load on a device

The device must be unlocked on the dashboard.

**Linux:** install udev rules from the repo (see `.vscode/20-ledger.ledgerblue.rules`), then from the builder container:

```shell
make load
```

**macOS / Windows:** use Python + `ledgerblue` or the VS Code load task; see [Ledger documentation](https://developers.ledger.com/).

## Tests

Full step-by-step guide (Docker, `BOLOS_SDK`, unit tests, Ragger, optional fuzz, CI parity): **[doc/RUN_TESTS.md](doc/RUN_TESTS.md)**. See also the Ledger portal [Getting started — Device App](https://developers.ledger.com/docs/device-app/getting-started).

Functional tests use [pytest](https://docs.pytest.org/) and [Ragger](https://github.com/LedgerHQ/ragger).

```shell
pip install -r tests/standalone/requirements.txt
PYTHONPATH=tests pytest tests/standalone/ --tb=short -v --device nanosp
```

Unit tests (require `BOLOS_SDK` for standard_app sources):

```shell
cd unit-tests && cmake -Bbuild -H. && cmake --build build && ctest --test-dir build
```

See [tests/standalone/README.md](tests/standalone/README.md) and [unit-tests/README.md](unit-tests/README.md).

## Documentation

| Document | Description |
| --- | --- |
| [APP_SPECIFICATION.md](APP_SPECIFICATION.md) | APDU protocol (authoritative) |
| [doc/APDU.md](doc/APDU.md) | Generic BOLOS APDU framing |
| [doc/TRANSACTION.md](doc/TRANSACTION.md) | Why the app does not serialize transactions |
| [doc/AI_ENGINEERING_STATUS.md](doc/AI_ENGINEERING_STATUS.md) | Implementation status and follow-ups |
| [doc/LEDGER_GUIDELINES_COMPLIANCE.md](doc/LEDGER_GUIDELINES_COMPLIANCE.md) | Ledger store / CI guidelines checklist |
| [doc/RUN_TESTS.md](doc/RUN_TESTS.md) | Step-by-step: run all tests (Docker, unit, Ragger, CI parity) |
| [doc/EXTERNAL_AUDIT.md](doc/EXTERNAL_AUDIT.md) | Scope and files for external security review |
| [SECURITY.md](SECURITY.md) | Vulnerability reporting |

API reference can be generated with Doxygen:

```shell
doxygen .doxygen/Doxyfile
```

## Continuous integration

GitHub Actions (when enabled on your fork) typically run: Ledger guidelines check, `clang-format`, multi-target build, unit tests, Ragger tests on Speculos, coverage, Doxygen. See `.github/workflows/`.

From the repo root, [`scripts/verify_local.sh`](scripts/verify_local.sh) runs a subset (Python `compileall`, optional `clang-format` if installed, optional unit tests when `BOLOS_SDK` is set, `pytest --collect-only` when pytest is available—use a venv with `tests/standalone/requirements.txt` if needed). A green run there does not replace CI; open a pull request and confirm all workflows pass on GitHub.

## Further reading

- [Ledger Developer Portal](https://developers.ledger.com/)
- Legacy reference app: [vba2000/app-waves](https://github.com/vba2000/app-waves/tree/version-2.0.1) (older protocol; not identical to this spec)
