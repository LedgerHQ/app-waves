from ragger.backend.interface import BackendInterface

from application_client.boilerplate_command_sender import BoilerplateCommandSender
from application_client.boilerplate_response_unpacker import unpack_get_app_configuration_response


def test_get_app_configuration(backend: BackendInterface) -> None:
    client = BoilerplateCommandSender(backend)
    rapdu = client.get_app_configuration()
    flags, mj, mn, pt, name = unpack_get_app_configuration_response(rapdu.data)
    assert flags == 0x01
    assert name == "Waves"
    assert (mj, mn, pt) == (2, 1, 0)
