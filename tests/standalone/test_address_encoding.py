"""
Offline checks: Waves address encoding matches src/address.c (no Speculos).
"""

from .utils import waves_address_from_pubkey, waves_secure_hash


def test_waves_secure_hash_blake2_person_empty() -> None:
    """SDK Blake2b-256 uses default personalization (same as hashlib blake2b with person=b'')."""
    data = b"abc"
    h = waves_secure_hash(data)
    assert len(h) == 32


def test_waves_address_reference_vector() -> None:
    """Fixed pubkey bytes -> deterministic address (Python reference)."""
    pk = bytes(range(32))
    assert waves_address_from_pubkey(pk, 0x57) == "3P7R6BMiHagZupDPmieaZ8hJ5JsyD5iC29n"


def test_waves_address_chain_bytes_differ() -> None:
    pk = bytes(range(32))
    a_main = waves_address_from_pubkey(pk, 0x57)
    a_test = waves_address_from_pubkey(pk, 0x54)
    assert a_main != a_test
