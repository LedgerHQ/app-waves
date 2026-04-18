from pathlib import Path
from typing import List
import re
import hashlib
import struct

import base58
from Crypto.Hash import keccak
from ragger.bip import pack_derivation_path

# Must match MAX_TX_STREAM_TOTAL in src/constants.h
MAX_TX_STREAM_TOTAL = 16 * 1024 * 1024


def sign_stream_init_payload(
    path: str, total_tx_len: int, first_tx_chunk: bytes = b""
) -> bytes:
    """SIGN_TX_STREAM INIT body: depth+path + total_be + optional first tx bytes."""
    return pack_derivation_path(path) + struct.pack(">I", total_tx_len) + first_tx_chunk


def waves_secure_hash(data: bytes) -> bytes:
    b2 = hashlib.blake2b(data, digest_size=32, person=b"").digest()
    k = keccak.new(digest_bits=256)
    k.update(b2)
    return k.digest()


def waves_address_from_pubkey(pubkey: bytes, chain_id: int) -> str:
    """
    Waves account address (Base58) from 32-byte Ed25519 public key.
    Must match firmware `waves_encode_address` in src/address.c (version 0x01,
    SecureHash(pubkey)[:20], Keccak256 checksum over 22-byte body).
    """
    if len(pubkey) != 32:
        raise ValueError("pubkey must be 32 bytes")
    if not 0 <= chain_id <= 255:
        raise ValueError("chain_id must be a byte")
    h32 = waves_secure_hash(pubkey)
    body = bytes([0x01, chain_id]) + h32[:20]
    k = keccak.new(digest_bits=256)
    k.update(body)
    chk = k.digest()[:4]
    raw = body + chk
    return base58.b58encode(raw).decode("ascii")


def check_ed25519_signature(public_key: bytes, signature: bytes, message: bytes) -> bool:
    try:
        from nacl.signing import VerifyKey
    except ImportError:
        raise RuntimeError("PyNaCl required for signature checks") from None
    if len(public_key) != 32 or len(signature) != 64:
        return False
    try:
        VerifyKey(public_key).verify(message, signature)
        return True
    except Exception:
        return False


def verify_name(name: str) -> None:
    name_str = ""
    lines = _read_makefile()
    name_re = re.compile(r'^APPNAME\s*=\s*"(?P<val>[^"]+)"', re.I)
    for line in lines:
        info = name_re.match(line.strip())
        if info:
            name_str = info.groupdict()["val"]
    assert name == name_str


def verify_version(version: str) -> None:
    vers_dict = {}
    vers_str = ""
    lines = _read_makefile()
    version_re = re.compile(r"^APPVERSION_(?P<part>\w)\s?=\s?(?P<val>\d*)", re.I)
    for line in lines:
        info = version_re.match(line)
        if info:
            dinfo = info.groupdict()
            vers_dict[dinfo["part"]] = dinfo["val"]
    try:
        vers_str = f"{vers_dict['M']}.{vers_dict['N']}.{vers_dict['P']}"
    except KeyError:
        pass
    assert version == vers_str


def _read_makefile() -> List[str]:
    parent = Path(__file__).parent.parent.parent.resolve()
    makefile = f"{parent}/Makefile"
    with open(makefile, "r", encoding="utf-8") as f_p:
        lines = f_p.readlines()
    return lines
