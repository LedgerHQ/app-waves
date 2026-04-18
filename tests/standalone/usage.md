# Ragger usage (WAVES Ledger app)

Tests can run on **Speculos** or a **USB device** (LedgerComm / LedgerWallet).

## Dependencies

```shell
pip install -r tests/standalone/requirements.txt
```

On Linux you may need:

```shell
sudo apt-get update && sudo apt-get install -y qemu-user-static
```

## Build the app

Use the Ledger builder image, mount this repository as `/app`, and build with the correct `BOLOS_SDK` for your device:

```shell
docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-lite:latest
docker run --user "$(id -u):$(id -g)" --rm -ti -v "$(realpath .):/app" --privileged -v "/dev/bus/usb:/dev/bus/usb" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-lite:latest
make clean && make BOLOS_SDK=$NANOSP_SDK
```

Replace `NANOSP_SDK` with `NANOX_SDK`, `STAX_SDK`, `FLEX_SDK`, or `APEX_SDK` as needed.

## Speculos

```shell
pytest tests/standalone/ -v --tb=short --device nanox --display
```

## Physical device

Load the app, open it on the device, then:

```shell
pytest tests/standalone/ -v --tb=short --device nanox --backend ledgerwallet
```

## Useful pytest flags

| Flag | Meaning |
| --- | --- |
| `-v` | Verbose summary |
| `-s` | Show prints; with Speculos + `DEBUG=1`, more app logs |
| `-k <name>` | Filter tests by name |
| `--tb=short` | Shorter tracebacks |
| `--device <name>` | `nanox`, `nanosp`, `stax`, `flex`, `all`, … |
| `--backend <name>` | `speculos` (default), `ledgercomm`, `ledgerwallet` |
| `--display` | Show Speculos UI (Qt) |
| `--golden_run` | Refresh golden screenshots instead of comparing |
| `--log_apdu_file <path>` | Log APDU traffic to a file |
