# Fuzzing (WAVES Ledger app)

## Purpose

Fuzzing feeds pseudo-random input to a small harness to find crashes, undefined behavior, or sanitizer violations.

This project’s target is **`fuzz_sign_stream_init`**: it parses the **SIGN_TX_STREAM INIT**-style prefix (path depth, 5 × uint32 BIP32 components, big-endian total size, optional bytes) using the same **buffer** helpers as the firmware, then runs **`waves_bip32_path_valid`** (same rules as the app). It does **not** link full BOLOS crypto. Dependencies are listed in `fuzzing/extra/SignStreamFuzzDeps.cmake`.

The harness implements:

`int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)`

If the fuzzer or a sanitizer reports a failure, the triggering input is saved (often as `crash-*`) for triage.

**Future hardening (optional):** a second harness could feed multi-chunk ADD/LAST sequences against a stubbed hash context to stress length and session transitions; the current target focuses on INIT parsing and path policy.

## Manual build (Ledger toolchain / Clang)

Use an environment where `BOLOS_SDK` points to [ledger-secure-sdk](https://github.com/LedgerHQ/ledger-secure-sdk) and **Clang** is available.

```shell
cd fuzzing
cmake -DBOLOS_SDK=/path/to/ledger-secure-sdk -DCMAKE_C_COMPILER=/usr/bin/clang -B build -S .
cmake --build build
./build/fuzz_sign_stream_init
```

## ClusterFuzzLite

CI builds the fuzzer via `.clusterfuzzlite/build.sh` and produces **`fuzz_sign_stream_init`** in the output directory.

General background: [ClusterFuzzLite](https://google.github.io/clusterfuzzlite/).

Example local image flow (adapt paths and image name to your setup):

```shell
mkdir -p fuzzing/corpus fuzzing/out
docker build -t waves-ledger-fuzz --file .clusterfuzzlite/Dockerfile .
```

The Dockerfile copies sources into the image; rebuild the image after code changes.
