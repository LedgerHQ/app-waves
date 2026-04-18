from ragger.backend.interface import BackendInterface

from application_client.boilerplate_command_sender import BoilerplateCommandSender
from application_client.boilerplate_response_unpacker import (
    unpack_get_app_name_response,
    unpack_get_version_response,
)

from .utils import verify_version, verify_name


def test_app_name_and_version_from_waves_ins(backend: BackendInterface) -> None:
    """INS 0xF4 app name + INS 0x03 version (replaces legacy BOLOS 0xB0 helper)."""
    client = BoilerplateCommandSender(backend)
    name_rsp = client.get_app_name()
    app_name = unpack_get_app_name_response(name_rsp.data)
    ver_rsp = client.get_version()
    major, minor, patch = unpack_get_version_response(ver_rsp.data)
    version = f"{major}.{minor}.{patch}"
    verify_name(app_name)
    verify_version(version)
