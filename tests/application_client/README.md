# Application client (functional tests)

Small Python helpers used by **pytest** to talk to the **WAVES** Ledger application. This is **not** a full wallet SDK.

## Role

- Encode APDUs (CLA `0xE0`, Waves INS set).
- Decode fixed response layouts (public key + address, configuration blob, signature).

## Contents

| File | Role |
| --- | --- |
| `boilerplate_command_sender.py` | APDU sender (class name kept for test imports; implements Waves protocol) |
| `boilerplate_response_unpacker.py` | Response parsers |
| `boilerplate_currency_utils.py` | Legacy path helper (may be unused; safe to remove if nothing imports it) |
| `tlv.py`, `pki_client.py` | Leftover helpers from boilerplate; unused by current WAVES standalone tests |
| `py.typed` | Typing marker |

Extend or replace these modules if you add new APDUs.

## Examples

See `tests/standalone/test_*.py`.
