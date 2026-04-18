"""
Corner cases for SIGN_TX_STREAM: state machine, limits, chain_id, chunk bounds.

Uses low-level APDU exchange (no UI) except where an async review is required.
"""

import pytest

from ragger.error import ExceptionRAPDU
from ragger.backend.interface import BackendInterface
from ragger.bip import pack_derivation_path

from application_client.boilerplate_command_sender import (
    BoilerplateCommandSender,
    Errors,
)
from .utils import MAX_TX_STREAM_TOTAL, sign_stream_init_payload

WAVES_PATH = "m/44'/5741564'/0'/0'/0'"
CHAIN_MAIN = 0x57


SWO_OK = 0x9000


def test_sign_last_without_init(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0x80, CHAIN_MAIN, b"")
    assert e.value.status == Errors.SWO_WAVES_SIGN_NO_INIT


def test_sign_init_total_zero(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    init = sign_stream_init_payload(WAVES_PATH, 0, b"")
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0, CHAIN_MAIN, init)
    assert e.value.status == Errors.SWO_INCORRECT_DATA


def test_sign_init_total_above_max(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    init = sign_stream_init_payload(WAVES_PATH, MAX_TX_STREAM_TOTAL + 1, b"")
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0, CHAIN_MAIN, init)
    assert e.value.status == Errors.SWO_INCORRECT_DATA


def test_sign_init_truncated_missing_total(backend: BackendInterface) -> None:
    """CData ends right after BIP32 path — cannot read 4-byte total."""
    client = BoilerplateCommandSender(backend)
    truncated = pack_derivation_path(WAVES_PATH)
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0, CHAIN_MAIN, truncated)
    assert e.value.status == Errors.SWO_WRONG_DATA_LENGTH


def test_sign_init_truncated_partial_total(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    truncated = pack_derivation_path(WAVES_PATH) + b"\x00\x01"
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0, CHAIN_MAIN, truncated)
    assert e.value.status == Errors.SWO_WRONG_DATA_LENGTH


def test_sign_invalid_p1(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0x02, CHAIN_MAIN, b"")
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2


def test_sign_wrong_chain_on_add_after_init(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    tx = b"abcd"
    init = sign_stream_init_payload(WAVES_PATH, len(tx), tx[:2])
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, init).status == SWO_OK
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(1, 0x54, tx[2:])
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2


def test_sign_wrong_chain_on_last_after_init(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    tx = b"ab"
    init = sign_stream_init_payload(WAVES_PATH, len(tx), tx)
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, init).status == SWO_OK
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0x80, 0x54, b"")
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2


def test_sign_add_chunk_overflows_total(backend: BackendInterface) -> None:
    """Cumulative payload length would exceed declared total."""
    client = BoilerplateCommandSender(backend)
    tx = b"abcdef"
    init = sign_stream_init_payload(WAVES_PATH, 3, tx[:2])
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, init).status == SWO_OK
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(1, CHAIN_MAIN, tx[2:6])
    assert e.value.status == Errors.SWO_INCORRECT_DATA


def test_sign_last_total_mismatch_short(backend: BackendInterface) -> None:
    """Declared total not reached when LAST runs."""
    client = BoilerplateCommandSender(backend)
    tx = b"abcde"
    init = sign_stream_init_payload(WAVES_PATH, len(tx), tx[:2])
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, init).status == SWO_OK
    assert client.sign_tx_stream_exchange(1, CHAIN_MAIN, tx[2:4]).status == SWO_OK
    with pytest.raises(ExceptionRAPDU) as e:
        client.sign_tx_stream_exchange(0x80, CHAIN_MAIN, b"")
    assert e.value.status == Errors.SWO_INCORRECT_DATA


def test_sign_second_init_overwrites_session(backend: BackendInterface) -> None:
    """New INIT clears previous streaming state (host recovery)."""
    client = BoilerplateCommandSender(backend)
    p1 = sign_stream_init_payload(WAVES_PATH, 10, b"abc")
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, p1).status == SWO_OK
    p2 = sign_stream_init_payload(WAVES_PATH, 2, b"xy")
    assert client.sign_tx_stream_exchange(0, CHAIN_MAIN, p2).status == SWO_OK
