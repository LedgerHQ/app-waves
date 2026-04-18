# WAVES Ledger Application — Technical Specification

**Firmware version:** 2.1.0 — must match `APPVERSION_M/N/P` in `Makefile` and `GET_VERSION` / `GET_APP_CONFIGURATION` responses.  
**Protocol specification revision:** 2.4 (APDU commands and payloads below; unchanged from prior spec drafts).  
**CLA:** `0xE0` for all application commands below  
**Transport:** USB HID or BLE (device-dependent), per Ledger BOLOS conventions  
**Max command data:** 255 bytes per APDU (`Lc`), typical Ledger limit

## 1. Purpose

The application is a **streaming cryptographic signer** for the Waves blockchain. It does **not** parse transaction payloads: it accumulates **SecureHash** in hardware and, after user approval, returns an **Ed25519** signature over that 32-byte digest. This supports arbitrarily large transactions (e.g. MassTransfer) within limited RAM.

**References (context only):** modern base — [Ledger app-boilerplate](https://github.com/LedgerHQ/app-boilerplate); historical Waves app — [vba2000/app-waves](https://github.com/vba2000/app-waves/tree/version-2.0.1) (legacy APDU differs from this document).

## 2. Cryptography

### 2.1 SecureHash

For a byte string `data` (the full serialized transaction bytes streamed over APDUs):

\[
\text{SecureHash}(data) = \text{Keccak256}(\text{Blake2b256}(data))
\]

- **Blake2b:** 256-bit (32-byte) digest.  
- **Keccak-256:** 32-byte digest (Ethereum-compatible Keccak, not FIPS SHA3).  

Implemented incrementally: only a **Blake2b context** and byte counters are kept while streaming; Keccak runs once on the final 32-byte Blake2b output.

### 2.2 Keys and path

| Item | Value |
| --- | --- |
| Curve | `CX_CURVE_Ed25519` |
| Derivation | SLIP-0010 Ed25519 (`HDW_ED25519_SLIP10`) |
| Standard path | `m / 44' / 5741564' / account' / 0' / address_index'` |
| Path depth in APDUs | **5** (fixed): `0x05` + 20 bytes (five big-endian `uint32_t`) |

The firmware **rejects** paths that are not exactly 5 levels, or that do not match `44'` (`0x8000002C`), Waves coin type hardened (`0x80000000 \| 5741564`), and **hardened** `account'`, `0'`, `address_index'`.

### 2.3 Signing

The device signs **SecureHash(tx)** with Ed25519 (SDK `*_no_throw` APIs). Response is the raw **64-byte** signature (no DER).

### 2.4 Address (GET_PUBLIC_KEY response)

The displayed / returned Waves address is **Base58**-encoded payload built from the 32-byte Ed25519 public key and the **chain ID** (P2), using the same **SecureHash** step on the public key material as used on-chain for account addressing (version byte + chain ID + 20-byte hash prefix + 4-byte checksum — see implementation in `src/address.c`).

## 3. Network (chain ID)

P2 is a single **network byte**. UI strings:

| P2 (hex) | ASCII | User-visible label |
| --- | --- | --- |
| `0x57` | `W` | Network: Mainnet |
| `0x54` | `T` | Network: Testnet |
| `0x53` | `S` | Network: Stagenet |
| other | — | `Network: Custom (0xXX)` |

The same P2 must be used consistently across **INIT / ADD / LAST** of one signing session; a mismatch aborts the session with an error.

## 4. Command overview

| INS | Name | P1 / P2 |
| --- | --- | --- |
| `0x03` | **GET_VERSION** (utility) | `P1=0`, `P2=0` |
| `0x04` | **GET_PUBLIC_KEY** | `P1`: `0x00` silent / `0x01` confirm; `P2`: chain ID |
| `0x06` | **GET_APP_CONFIGURATION** | `P1=0`, `P2=0` |
| `0x08` | **SIGN_TX_STREAM** | `P1`: `0x00` INIT / `0x01` ADD / `0x80` LAST; `P2`: chain ID |
| `0xF4` | **GET_APP_NAME** (utility) | `P1=0`, `P2=0` |

---

## 5. GET_APP_CONFIGURATION — INS `0x06`

Ledger Live / Waves Keeper use this to detect capabilities and versioning.

### Command

| Field | Value |
| --- | --- |
| CLA | `E0` |
| INS | `06` |
| P1 | `00` |
| P2 | `00` |
| Data | *(empty)* |

### Response data

| Offset | Size | Description |
| --- | --- | --- |
| 0 | 1 | Flags: `0x01` = blind signing capable |
| 1 | 1 | Major version |
| 2 | 1 | Minor version |
| 3 | 1 | Patch version |
| 4 | 1 | App name length `L` |
| 5 | `L` | App name ASCII (e.g. `Waves`) |

### Status

`0x9000` on success. See §10 for common errors.

---

## 6. GET_PUBLIC_KEY — INS `0x04`

### Command

| Field | Value |
| --- | --- |
| CLA | `E0` |
| INS | `04` |
| P1 | `00` silent / `01` show address and confirm |
| P2 | Chain ID (network byte) |
| Data | See below |

### Input data

| Offset | Size | Description |
| --- | --- | --- |
| 0 | 1 | Path levels — must be `0x05` |
| 1 | 20 | Five BIP32 components, each **big-endian** `uint32_t` |

### Response data (after `0x9000`)

| Offset | Size | Description |
| --- | --- | --- |
| 0 | 32 | Ed25519 public key |
| 32 | 1 | Address string length `A` |
| 33 | `A` | Waves address, ASCII Base58 (no NUL terminator) |

If `P1=01`, the user must approve on device before the response is sent.

---

## 7. GET_VERSION — INS `0x03` (utility)

| Field | Value |
| --- | --- |
| P1, P2 | `00` |
| Data | empty |

**Response:** 3 bytes — major, minor, patch (same semantics as Makefile `APPVERSION_M/N/P`).

---

## 8. GET_APP_NAME — INS `0xF4` (utility)

| Field | Value |
| --- | --- |
| P1, P2 | `00` |
| Data | empty |

**Response:** raw ASCII application name (length = `APPNAME_LEN`), no length prefix.

---

## 9. SIGN_TX_STREAM — INS `0x08`

Streaming only: the device **must not** buffer the full transaction. It updates Blake2b over each chunk until **LAST**, then computes Keccak256(Blake2b digest), shows **Base58(SecureHash)** and network on screen, and on approval returns the signature.

### 9.1 INIT — `P1 = 0x00`

| Field | Value |
| --- | --- |
| P2 | Chain ID |

#### Input data layout

| Offset | Size | Description |
| --- | --- | --- |
| 0 | 1 | `0x05` (path depth) |
| 1 | 20 | BIP32 path (5 × uint32 BE), must pass §2.2 rules |
| 21 | 4 | Total transaction size **big-endian** `uint32_t` |
| 25 | var | First chunk of raw transaction (may be empty only if total size is 0 — **note:** total `0` is rejected by firmware) |

**Rules:**

- `total` must satisfy `0 < total ≤ MAX_TX_STREAM_TOTAL` (16 MiB in current firmware).
- After INIT, intermediate state is **active** until success, error, or user reject.

**Response:** `0x9000` with empty data (intermediate acknowledgements).

### 9.2 ADD — `P1 = 0x01`

| P2 | Same chain ID as INIT |

**Data:** next chunk of raw transaction (length ≤ 255). Cumulative received bytes after this APDU must not exceed `total`.

**Response:** `0x9000`, empty data.

**If no successful INIT:** `0x6B00` (`SWO_WAVES_SIGN_NO_INIT`).

### 9.3 LAST — `P1 = 0x80`

| P2 | Same chain ID as INIT |

**Data:** final chunk of raw transaction (may be empty if the last bytes were already sent in ADD).

After processing:

- Cumulative length must equal `total`.
- SecureHash is finalized; UI is shown; on **Approve**, response is **64 bytes** Ed25519 signature.

**If no active session:** `0x6B00`.

**Response (success):** 64-byte signature + `0x9000`.

---

## 10. Status words

Standard Ledger / application mapping (non-exhaustive; see `status_words.h` in SDK for full set):

| SW | Name | Typical use in this app |
| --- | --- | --- |
| `9000` | OK | Success |
| `6985` | CONDITIONS_NOT_SATISFIED | User rejected, or command refused in current state |
| `6A80` | INCORRECT_DATA | Bad path, hash update failure, size mismatch at LAST, etc. |
| `6A86` | INCORRECT_P1_P2 | Invalid P1/P2 for command; chain ID mismatch mid stream |
| `6A87` | WRONG_DATA_LENGTH | Bad `Lc` / truncated payload |
| `6B00` | WAVES_SIGN_NO_INIT | ADD or LAST without prior successful INIT |
| `6D00` | INVALID_INS | Unknown INS |
| `6E00` | INVALID_CLA | CLA ≠ `E0` |
| *see SDK* | SECURITY_ISSUE | Key derivation / signing failure (`SWO_SECURITY_ISSUE` in `status_words.h`) |

---

## 11. Host integration notes

1. **Chunking:** Split the serialized transaction across INIT (header + first bytes) and ADD/LAST so each APDU `Lc ≤ 255`. The **LAST** APDU carries the **final** segment (can be length 0).
2. **Signing session:** Do not interleave other commands that reset app context between INIT and LAST if the host expects a continuous session (firmware may clear state on other handlers).
3. **Verification off-device:** Recompute SecureHash with the same Blake2b-256 + Keccak-256 definition and verify Ed25519 against the returned public key for the same path.

---

## 12. Related files

| Area | Location |
| --- | --- |
| APDU dispatch | `src/apdu/dispatcher.c` |
| Constants / INS | `src/constants.h` |
| SecureHash | `src/crypto/secure_hash.c` |
| Sign stream | `src/handler/sign_tx_stream.c` |
| Public key / address | `src/handler/get_public_key.c`, `src/address.c` |
| App configuration | `src/handler/get_app_configuration.c` |
| Engineering / AI summary | `doc/AI_ENGINEERING_STATUS.md` |
