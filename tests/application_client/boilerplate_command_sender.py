from enum import IntEnum
from typing import Generator, List, Optional
from contextlib import contextmanager
import struct

from ragger.backend.interface import BackendInterface, RAPDU
from ragger.bip import pack_derivation_path
from ragger.error import StatusWords

MAX_APDU_LEN: int = 255

CLA: int = 0xE0


class P1(IntEnum):
    P1_START = 0x00
    P1_CONFIRM = 0x01


class P2(IntEnum):
    P2_NONE = 0x00


class InsType(IntEnum):
    GET_VERSION = 0x03
    GET_PUBLIC_KEY = 0x04
    GET_APP_CONFIGURATION = 0x06
    SIGN_TX_STREAM = 0x08
    GET_APP_NAME_UTIL = 0xF4


custom_errors = {
    "SWO_WAVES_SIGN_NO_INIT": 0x6B00,
    "SWO_INCORRECT_DATA": 0x6A80,
    "SWO_INCORRECT_P1_P2": 0x6A86,
    "SWO_WRONG_DATA_LENGTH": 0x6A87,
}

_errors_dict = {m.name: m.value for m in StatusWords}
_errors_dict.update(custom_errors)
Errors = IntEnum("Errors", _errors_dict)  # type: ignore[misc]


def split_message(message: bytes, max_size: int) -> List[bytes]:
    return [message[x : x + max_size] for x in range(0, len(message), max_size)]


class BoilerplateCommandSender:
    def __init__(self, backend: BackendInterface) -> None:
        self.backend = backend

    def get_version(self) -> RAPDU:
        return self.backend.exchange(
            cla=CLA, ins=InsType.GET_VERSION, p1=0, p2=0, data=b""
        )

    def get_app_name(self) -> RAPDU:
        return self.backend.exchange(
            cla=CLA, ins=InsType.GET_APP_NAME_UTIL, p1=0, p2=0, data=b""
        )

    def get_app_configuration(self) -> RAPDU:
        return self.backend.exchange(
            cla=CLA, ins=InsType.GET_APP_CONFIGURATION, p1=0, p2=0, data=b""
        )

    def get_public_key(self, path: str, chain_id: int = 0x57) -> RAPDU:
        return self.backend.exchange(
            cla=CLA,
            ins=InsType.GET_PUBLIC_KEY,
            p1=0,
            p2=chain_id,
            data=pack_derivation_path(path),
        )

    @contextmanager
    def get_public_key_with_confirmation(
        self, path: str, chain_id: int = 0x57
    ) -> Generator[None, None, None]:
        with self.backend.exchange_async(
            cla=CLA,
            ins=InsType.GET_PUBLIC_KEY,
            p1=P1.P1_CONFIRM,
            p2=chain_id,
            data=pack_derivation_path(path),
        ) as response:
            yield response

    @contextmanager
    def sign_tx_stream(
        self, path: str, tx: bytes, chain_id: int = 0x57
    ) -> Generator[None, None, None]:
        total = len(tx)
        path_enc = pack_derivation_path(path)
        overhead = len(path_enc) + 4
        if overhead > MAX_APDU_LEN:
            raise ValueError("path too long for APDU")
        first_sz = min(len(tx), MAX_APDU_LEN - overhead)
        first = tx[:first_sz]
        rest = tx[first_sz:]
        init_payload = path_enc + struct.pack(">I", total) + first
        self.backend.exchange(
            cla=CLA,
            ins=InsType.SIGN_TX_STREAM,
            p1=0x00,
            p2=chain_id,
            data=init_payload,
        )
        add_batches: List[bytes] = []
        rem = rest
        while len(rem) > MAX_APDU_LEN:
            add_batches.append(rem[:MAX_APDU_LEN])
            rem = rem[MAX_APDU_LEN:]
        for ch in add_batches:
            self.backend.exchange(
                cla=CLA,
                ins=InsType.SIGN_TX_STREAM,
                p1=0x01,
                p2=chain_id,
                data=ch,
            )
        with self.backend.exchange_async(
            cla=CLA,
            ins=InsType.SIGN_TX_STREAM,
            p1=0x80,
            p2=chain_id,
            data=rem,
        ) as response:
            yield response

    def get_async_response(self) -> Optional[RAPDU]:
        return self.backend.last_async_response

    def sign_tx_stream_exchange(
        self, p1: int, p2: int, data: bytes = b""
    ) -> RAPDU:
        """Low-level SIGN_TX_STREAM (0x08); use for corner-case tests."""
        return self.backend.exchange(
            cla=CLA, ins=InsType.SIGN_TX_STREAM, p1=p1, p2=p2, data=data
        )
