# Unit tests (WAVES)

## Prerequisites

- CMake >= 3.10
- CMocka >= 1.1.5
- For coverage: lcov >= 1.14
- Environment variable **`BOLOS_SDK`** pointing to [ledger-secure-sdk](https://github.com/LedgerHQ/ledger-secure-sdk) (for `lib_standard_app` sources: `buffer.c`, `bip32.c`, etc.)

## Configure and build

```shell
export BOLOS_SDK=/path/to/ledger-secure-sdk
cd unit-tests
cmake -B build -S .
cmake --build build
```

## Run

```shell
CTEST_OUTPUT_ON_FAILURE=1 ctest --test-dir build
```

## Coverage

If `gen_coverage.sh` is present:

```shell
./gen_coverage.sh
```

Open `coverage/index.html` when generated.

Current tests focus on **buffer parsing** for sign-stream INIT layout (`test_buffer_init_layout.c`).
