import pytest

from ragger.error import ExceptionRAPDU
from ragger.backend.interface import BackendInterface

from application_client.boilerplate_command_sender import CLA, InsType, P1, Errors


def test_get_public_key_empty_data(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_PUBLIC_KEY, p1=0, p2=0x57, data=b"")
    assert e.value.status == Errors.SWO_WRONG_DATA_LENGTH


def test_bad_cla(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA + 1, ins=InsType.GET_VERSION)
    assert e.value.status == Errors.SWO_INVALID_CLA


def test_bad_ins(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=0xFF)
    assert e.value.status == Errors.SWO_INVALID_INS


def test_wrong_p1p2(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=1, p2=0)
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_VERSION, p1=0, p2=1)
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(cla=CLA, ins=InsType.GET_APP_NAME_UTIL, p1=1, p2=0)
    assert e.value.status == Errors.SWO_INCORRECT_P1_P2


def test_wrong_data_length(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(bytes.fromhex("E00300"))
    assert e.value.status == Errors.SWO_WRONG_DATA_LENGTH
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange_raw(bytes.fromhex("E003000005"))
    assert e.value.status == Errors.SWO_WRONG_DATA_LENGTH


def test_sign_stream_without_init(backend: BackendInterface) -> None:
    with pytest.raises(ExceptionRAPDU) as e:
        backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX_STREAM,
            p1=0x01,
            p2=0x57,
            data=b"abcde",
        )
    assert e.value.status == Errors.SWO_WAVES_SIGN_NO_INIT
