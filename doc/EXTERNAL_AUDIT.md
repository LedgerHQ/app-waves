# External security audit — scope and orientation

This document helps auditors navigate the **WAVES** Ledger application (**firmware 2.1.0**, APDU protocol spec **revision 2.4**) and reproduce builds/tests.

## Product scope

| In scope | Out of scope |
|----------|----------------|
| APDU protocol in [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) | Full transaction parsing on device |
| Streaming **SecureHash** = `Keccak256(Blake2b256(tx))` | Ledger Exchange / swap (`os_lib_call`) |
| Ed25519 signatures (SLIP-0010), path `m/44'/5741564'/…'` | Host wallet UI, node software |
| Blind signing UI (hash + network) | Parsing transaction semantics |

**Device targets** (see [ledger_app.toml](../ledger_app.toml)): Nano X, Nano S+, Stax, Flex, Apex P. Legacy Nano S (`nanos`) is not listed in the manifest; builds focus on supported modern targets.

## Trust boundaries

- **Trusted:** BOLOS firmware, `ledger-secure-sdk` cryptographic APIs, user confirmation on device.
- **Untrusted:** Host computer sending APDUs; must not be able to extract seed or sign without user approval for the presented review screen.

## Module map (full tree)

See [AUDIT_MODULE_MAP.md](AUDIT_MODULE_MAP.md) for a per-file description of all `src/` modules and audit notes.

## Security review notes

Structured threat model, review pass summary, and sign-off template: [SECURITY_REVIEW_NOTES.md](SECURITY_REVIEW_NOTES.md).

## Files to review first

| Area | Path |
|------|------|
| APDU dispatch | [src/apdu/dispatcher.c](../src/apdu/dispatcher.c) |
| Sign streaming + session abort | [src/handler/sign_tx_stream.c](../src/handler/sign_tx_stream.c) |
| SecureHash | [src/crypto/secure_hash.c](../src/crypto/secure_hash.c), [src/crypto/path_policy.c](../src/crypto/path_policy.c) |
| Public key / address | [src/handler/get_public_key.c](../src/handler/get_public_key.c), [src/address.c](../src/address.c) |
| User approve/reject | [src/ui/action/validate.c](../src/ui/action/validate.c) |
| Responses | [src/helper/send_response.c](../src/helper/send_response.c) |

## Reproducible build

Use Ledger’s Docker image and `BOLOS_SDK` for the target (see [README.md](../README.md)):

```shell
make BOLOS_SDK=$NANOSP_SDK  # example
```

## Tests and evidence

| Layer | Command / location |
|-------|---------------------|
| **End-to-end procedure** | [RUN_TESTS.md](RUN_TESTS.md) (Docker, `BOLOS_SDK`, order of commands, CI parity) |
| Functional (Ragger) | `PYTHONPATH=tests pytest tests/standalone/ -v --device nanosp` (after `pip install -r tests/standalone/requirements.txt`) |
| Unit (cmocka) | `cd unit-tests && cmake -B build -S . && cmake --build build && ctest --test-dir build` (requires `BOLOS_SDK`) |
| Fuzz | [fuzzing/README.md](../fuzzing/README.md) |
| Ledger CI checklist | [LEDGER_GUIDELINES_COMPLIANCE.md](LEDGER_GUIDELINES_COMPLIANCE.md) |

## Known limitations

See [AI_ENGINEERING_STATUS.md](AI_ENGINEERING_STATUS.md): independent confirmation of Waves address rules on live networks, Ragger snapshot refresh after UI copy changes, host integration with Ledger Live / Waves Keeper.

## Coordinated disclosure

See [SECURITY.md](../SECURITY.md).
