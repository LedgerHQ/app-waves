# Security review notes (internal / auditor handoff)

This document records the **threat model**, **mitigations**, **residual risks**, and a **structured review pass** over the WAVES Ledger app firmware. It complements [EXTERNAL_AUDIT.md](EXTERNAL_AUDIT.md) and [AUDIT_MODULE_MAP.md](AUDIT_MODULE_MAP.md).

**Scope:** On-device C code under `src/`, APDU protocol in [APP_SPECIFICATION.md](../APP_SPECIFICATION.md).  
**Out of scope:** Host wallets, Ledger Live listing, Waves consensus, third-party nodes.

---

## 1. Threat model

| Asset | Adversary | Goal |
|-------|-----------|------|
| User seed / private key | Malicious or buggy host | Extract key material or sign without user consent for the shown review. |
| Signature correctness | Malicious host | Obtain a valid signature for a **different** message than the user intends. |

**Trust:** BOLOS, `ledger-secure-sdk` crypto APIs, and explicit user actions on the device screen.  
**Untrusted:** All APDU input from the host.

**Blind signing:** The device displays **network** and **transaction hash** (SecureHash, Base58). It does **not** parse transaction fields. The user approves signing **the hash of the byte stream** the host sent. If that stream is not the intended serialized transaction, the signature may still be cryptographically valid for the hash shown — integrators must serialize bytes exactly as the network expects. See [TRANSACTION.md](TRANSACTION.md).

---

## 2. Security-relevant properties (verified in code review)

| Property | Implementation |
|----------|----------------|
| No signing without UI approval | [`validate.c`](../src/ui/action/validate.c): Ed25519 sign only after user confirms blind-sign; pubkey export only after address confirm when `display=1`. |
| Session isolation | [`sign_tx_stream.c`](../src/handler/sign_tx_stream.c): `chain_id` checked on ADD/LAST; `total_expected` enforced on LAST; session cleared on error; streaming stopped before UI so hash cannot change during review. |
| Path policy | [`path_policy.c`](../src/crypto/path_policy.c): fixed depth; hardened-only path segments per Waves policy. |
| Secret / context clearing | `explicit_bzero` on `G_context` and stack secrets on failure and after successful responses where applicable. |
| APDU validation | [`dispatcher.c`](../src/apdu/dispatcher.c): CLA, INS, P1/P2, buffer presence for GET_PUBLIC_KEY. |

---

## 3. Recommendations addressed in repository

| Recommendation | Action |
|----------------|--------|
| Document blind-sign semantics for users and integrators | [TRANSACTION.md](TRANSACTION.md) — section *Threat model (blind signing)*. |
| Reproducible Python test stack | [tests/standalone/requirements.txt](../tests/standalone/requirements.txt) — upper bounds on major versions to reduce CI drift; regenerate a lockfile in CI or locally if stricter pinning is required. |
| Type clarity for hash context | [`types.h`](../src/types.h) uses `waves_secure_hash_ctx_t` in `sign_stream_ctx_t` (aligned with [`secure_hash.h`](../src/crypto/secure_hash.h)). |
| Auditor navigation | [AUDIT_MODULE_MAP.md](AUDIT_MODULE_MAP.md), [EXTERNAL_AUDIT.md](EXTERNAL_AUDIT.md) cross-links. |

---

## 4. Residual risks (accepted / tracked)

| Risk | Mitigation / note |
|------|-------------------|
| Host sends wrong bytes but correct encoding | User sees hash; responsibility shared with host — document in [TRANSACTION.md](TRANSACTION.md). |
| Waves address / SecureHash vs live network | Cross-check with Waves docs and tests; Python reference tests in `tests/standalone/` exercise address encoding. |
| Ragger `test_sign_tx_stream_short` flaky (`ChunkedEncodingError`) | Marked `xfail(strict=False)`; tracks Speculos HTTP behavior, not firmware logic. Revisit when bumping `ragger`/`speculos`. |
| Fuzz coverage | [`fuzzing/`](../fuzzing/) currently targets INIT-heavy paths; extending harness to multi-chunk ADD/LAST remains optional hardening. |

---

## 5. Review pass (second pass, post-remediation)

**Date:** 2026-04-03 (documentation and dependency-metadata update).

**Method:** Static review of `src/apdu/dispatcher.c`, `src/handler/sign_tx_stream.c`, `src/handler/get_public_key.c`, `src/ui/action/validate.c`, `src/crypto/{secure_hash,path_policy}.c`, `src/address.c`, NBGL display files, `src/helper/send_response.c`.

**Result:** **No new blocking security findings** in the reviewed paths. Previous observations (blind-sign semantics, off-device Waves alignment, CI flake on Speculos) are **documented** in sections 1, 4, and [TRANSACTION.md](TRANSACTION.md).

**Sign-off (template):**

| Role | Name | Date | Signature |
|------|------|------|-----------|
| Development | | | |
| Security review | | | |

---

## 6. References

- [SECURITY.md](../SECURITY.md) — disclosure policy.  
- [APP_SPECIFICATION.md](../APP_SPECIFICATION.md) — APDU protocol.  
- [RUN_TESTS.md](RUN_TESTS.md) — how to reproduce tests in Docker.
