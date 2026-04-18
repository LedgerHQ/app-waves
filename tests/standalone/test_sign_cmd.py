import pytest

from ledgered.devices import Device
from ragger.error import ExceptionRAPDU
from ragger.backend.interface import BackendInterface
from ragger.navigator.navigation_scenario import NavigateWithScenario

from application_client.boilerplate_command_sender import BoilerplateCommandSender, Errors
from application_client.boilerplate_response_unpacker import (
    unpack_waves_get_public_key_response,
    unpack_waves_sign_response,
)
from .utils import waves_secure_hash, check_ed25519_signature

WAVES_PATH = "m/44'/5741564'/0'/0'/0'"
BAD_PATH_NON_HARDENED_ACCOUNT = "m/44'/5741564'/0/0'/0'"


def test_sign_tx_stream_rejects_non_hardened_path(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_tx_stream(path=BAD_PATH_NON_HARDENED_ACCOUNT, tx=b"x"):
            pass
    assert e.value.status == Errors.SWO_INCORRECT_DATA


@pytest.mark.xfail(
    strict=False,
    reason="Speculos HTTP: ChunkedEncodingError while polling async response during blind-sign navigation (known flake).",
)
def test_sign_tx_stream_short(
    backend: BackendInterface,
    scenario_navigator: NavigateWithScenario,
    device: Device,
) -> None:
    client = BoilerplateCommandSender(backend)
    rapdu = client.get_public_key(path=WAVES_PATH)
    pk, _addr = unpack_waves_get_public_key_response(rapdu.data)

    tx = b"hello waves ledger"
    msg_hash = waves_secure_hash(tx)

    blind_anchor = None
    if device.touchable:
        blind_anchor = r"^(Blind signing|Sign transaction hash\?|Hold to sign)$"

    with client.sign_tx_stream(path=WAVES_PATH, tx=tx):
        scenario_navigator.review_approve_with_warning(
            warning_path="part1", custom_screen_text=blind_anchor
        )

    raw = client.get_async_response().data
    sig = unpack_waves_sign_response(raw)
    assert check_ed25519_signature(pk, sig, msg_hash)


def test_sign_tx_stream_refused(
    backend: BackendInterface,
    scenario_navigator: NavigateWithScenario,
    device: Device,
) -> None:
    client = BoilerplateCommandSender(backend)
    tx = b"nope"

    with pytest.raises(ExceptionRAPDU) as e:
        with client.sign_tx_stream(path=WAVES_PATH, tx=tx):
            if device.touchable:
                # Same blind-sign warning as approve; then swipe until Hold to sign, then reject.
                scenario_navigator.review_reject_with_warning(
                    warning_path="part1",
                    nb_warnings=1,
                )
            else:
                scenario_navigator.review_reject()

    assert e.value.status == Errors.SWO_CONDITIONS_NOT_SATISFIED
