from typing import Tuple
from struct import unpack


def pop_sized_buf_from_buffer(buffer: bytes, size: int) -> Tuple[bytes, bytes]:
    return buffer[size:], buffer[0:size]


def pop_size_prefixed_buf_from_buf(buffer: bytes) -> Tuple[bytes, int, bytes]:
    data_len = buffer[0]
    return buffer[1 + data_len :], data_len, buffer[1 : data_len + 1]


def unpack_get_app_name_response(response: bytes) -> str:
    return response.decode("ascii")


def unpack_get_version_response(response: bytes) -> Tuple[int, int, int]:
    assert len(response) == 3
    major, minor, patch = unpack("BBB", response)
    return (major, minor, patch)


def unpack_get_app_configuration_response(response: bytes) -> Tuple[int, int, int, int, str]:
    assert len(response) >= 5
    flags, mj, mn, pt, nl = unpack("BBBBB", response[:5])
    name = response[5 : 5 + nl].decode("ascii")
    assert len(response) == 5 + nl
    return flags, mj, mn, pt, name


def unpack_waves_get_public_key_response(response: bytes) -> Tuple[bytes, str]:
    assert len(response) >= 33
    pk = response[0:32]
    alen = response[32]
    addr = response[33 : 33 + alen].decode("ascii").strip()
    assert len(response) == 33 + alen
    return pk, addr


def unpack_waves_sign_response(response: bytes) -> bytes:
    assert len(response) == 64
    return response
