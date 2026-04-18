from ledgered.devices import Device
from ragger.navigator import Navigator, NavInsID, NavIns


def test_app_mainmenu(
    device: Device,
    navigator: Navigator,
    test_name: str,
    default_screenshot_path: str,
) -> None:
    """Capture home/settings baseline; update golden images when UI changes."""
    instructions = []
    if device.is_nano:
        instructions += [
            NavInsID.RIGHT_CLICK,
            NavInsID.BOTH_CLICK,
        ]
    elif device.touchable:
        # Empty settings (no switch rows): boilerplate dummy-switch taps do not hit any control.
        # Open settings then exit via header back (ragger UseCaseSettings positions).
        instructions += [
            NavInsID.USE_CASE_HOME_SETTINGS,
            NavInsID.USE_CASE_SETTINGS_MULTI_PAGE_EXIT,
        ]

    # Headless touch devices can be slow to refresh after NBGL navigation.
    nav_timeout = 180.0 if not device.is_nano else 10.0
    navigator.navigate_and_compare(
        default_screenshot_path,
        test_name,
        instructions,
        timeout=nav_timeout,
        screen_change_before_first_instruction=False,
    )
