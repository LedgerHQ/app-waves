#!/usr/bin/env bash
# Local checks aligned with Ledger CI (run from repo root; BOLOS_SDK optional for unit tests).
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
cd "$ROOT"

echo "== Python syntax (application_client + standalone tests) =="
python3 -m compileall -q tests/application_client tests/standalone || true

if command -v clang-format >/dev/null 2>&1; then
  echo "== clang-format (check) =="
  mapfile -t CF < <(find src -type f \( -name '*.c' -o -name '*.h' \))
  if ((${#CF[@]})); then
    clang-format --dry-run --Werror "${CF[@]}"
  fi
else
  echo "SKIP: clang-format not installed (install clang-format or use Ledger Docker image)."
fi

if [[ -n "${BOLOS_SDK:-}" && -f "$BOLOS_SDK/lib_standard_app/buffer.c" ]]; then
  echo "== Unit tests (cmocka) =="
  cmake -S unit-tests -B unit-tests/build
  cmake --build unit-tests/build
  ctest --test-dir unit-tests/build --output-on-failure
else
  echo "SKIP: unit-tests (set BOLOS_SDK to ledger-secure-sdk path)."
fi

PY=python3
if [[ -x "$ROOT/.venv/bin/python" ]]; then
  PY="$ROOT/.venv/bin/python"
fi
if "$PY" -c "import pytest" 2>/dev/null; then
  echo "== pytest collect (requires --device; using nanosp) =="
  PYTHONPATH=tests "$PY" -m pytest tests/standalone/ --collect-only -q --device nanosp
else
  echo "SKIP: pytest not installed (python3 -m venv .venv && .venv/bin/pip install -r tests/standalone/requirements.txt)."
fi

echo "Done. Full parity: push a branch and confirm GitHub Actions (guidelines_enforcer, build, lint)."
