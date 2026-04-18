import pytest

from ragger.error import ExceptionRAPDU
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario

from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import unpack_waves_get_public_key_response

from .utils import waves_address_from_pubkey

WAVES_PATH = "m/44'/5741564'/0'/0'/0'"


def test_get_public_key_no_confirm(backend: BackendInterface) -> None:
    """Address string must match independent derivation from pubkey (same as legacy Waves tooling)."""
    client = BoilerplateCommandSender(backend)
    response = client.get_public_key(path=WAVES_PATH).data
    pk, addr = unpack_waves_get_public_key_response(response)
    assert len(pk) == 32
    assert len(addr) >= 20
    assert addr == waves_address_from_pubkey(pk, 0x57)


def test_get_public_key_confirm_accepted(
    backend: BackendInterface, scenario_navigator: NavigateWithScenario
) -> None:
    client = BoilerplateCommandSender(backend)
    with client.get_public_key_with_confirmation(path=WAVES_PATH):
        scenario_navigator.address_review_approve()

    response = client.get_async_response().data
    pk, addr = unpack_waves_get_public_key_response(response)
    assert len(pk) == 32
    assert len(addr) >= 20
    assert addr == waves_address_from_pubkey(pk, 0x57)


def test_get_public_key_rejects_non_hardened_path(backend: BackendInterface) -> None:
    """Spec §2.2: account' must be hardened; m/44'/5741564'/0/0'/0' must fail."""
    client = BoilerplateCommandSender(backend)
    bad_path = "m/44'/5741564'/0/0'/0'"
    with pytest.raises(ExceptionRAPDU) as e:
        client.get_public_key(path=bad_path)
    assert e.value.status == Errors.SWO_INCORRECT_DATA
    assert len(e.value.data) == 0


def test_get_public_key_confirm_refused(
    backend: BackendInterface, scenario_navigator: NavigateWithScenario
) -> None:
    client = BoilerplateCommandSender(backend)

    with pytest.raises(ExceptionRAPDU) as e:
        with client.get_public_key_with_confirmation(path=WAVES_PATH):
            scenario_navigator.address_review_reject()

    assert e.value.status == Errors.SWO_CONDITIONS_NOT_SATISFIED
    assert len(e.value.data) == 0
