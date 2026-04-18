# Transaction handling in the WAVES Ledger app

This application **does not define or parse** a custom transaction binary format on the device.

## What the firmware does

1. The host sends the **raw serialized Waves transaction** in chunks via **SIGN_TX_STREAM** (`INS 0x08`): see [APP_SPECIFICATION.md](../APP_SPECIFICATION.md).
2. The device updates **SecureHash** incrementally: `Keccak256(Blake2b256(transaction_bytes))`.
3. After the user approves the blind-sign screen (network + hash in Base58), the device returns a **64-byte Ed25519** signature over the 32-byte SecureHash.

There is **no** `nonce` / `to` / `value` parsing, no token metadata, and **no** full-transaction RAM buffer beyond hash context and counters.

## Threat model (blind signing)

The firmware **does not** validate that the streamed bytes are a well-formed Waves transaction or match a particular intent (transfer, invoke script, etc.). It computes **SecureHash** over **exactly** the bytes received and shows that hash (and network) on screen. The user approves signing **that hash**.

**Implications:**

- A malicious or compromised host could stream bytes whose hash the user does not associate with their intended action. The device still enforces *user consent for the displayed hash*, not *semantic correctness of the transaction*.
- Integrators and end-user documentation should state clearly: security of the **meaning** of the transaction depends on **trusted serialization** on the host and verification against [Waves documentation](https://docs.waves.tech/).

## Where the transaction format is defined

The structure of Waves transactions is defined by the **Waves protocol** and node/SDK documentation (e.g. [Waves documentation](https://docs.waves.tech/)), not by this repository.

Integrators must:

- Serialize transactions exactly as the network expects.
- Stream those bytes to the Ledger app.
- Recompute SecureHash off-device to verify signatures.

## Related source

- Signing and chunking: `src/handler/sign_tx_stream.c`
- Hash pipeline: `src/crypto/secure_hash.c`
