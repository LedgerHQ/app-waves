# Application Protocol Data Unit (APDU)

This page describes **generic** BOLOS APDU framing. For **WAVES application** commands (CLA, INS, payloads, status words), use [APP_SPECIFICATION.md](../APP_SPECIFICATION.md).

Communication follows [BOLOS](https://ledger.readthedocs.io/en/latest/bolos/overview.html) conventions, close to [ISO 7816-4](https://www.iso.org/standard/77180.html) with Ledger-specific rules:

- `Lc` is always exactly 1 byte.
- There is no `Le` field in the command APDU.
- Maximum command size: 5-byte header + 255 bytes of data (260 bytes total).
- Maximum response size: up to 258 bytes of data + 2-byte status word (260 bytes total).

Status words are broadly aligned with common [APDU response codes](https://www.eftlab.com/knowledge-base/complete-list-of-apdu-responses/).

## Command APDU

| Field | Length (bytes) | Description |
| --- | --- | --- |
| CLA | 1 | Instruction class |
| INS | 1 | Instruction code |
| P1 | 1 | Parameter 1 |
| P2 | 1 | Parameter 2 |
| Lc | 1 | Number of bytes in command data (0–255) |
| CData | var | Payload (`Lc` bytes) |

## Response APDU

| Field | Length (bytes) | Description |
| --- | --- | --- |
| RData | var | Response data (may be empty) |
| SW | 2 | Status word (e.g. `0x9000` success) |
