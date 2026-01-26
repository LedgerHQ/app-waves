from application_client.waves_sign_data import WavesSignData, WavesTestEngine, SIGNED_CODES
import base58

# In this tests we check the behavior of the device when asked to sign a transaction
PATH = "m/44'/5741564'/0'/0'/1'"
chain_id = ord("W")  # Mainnet

def test_data_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    sign_data = bytes("test data to sign", "utf-8")
    encoded_bytes = base58.b58encode(sign_data)
    encoded_str = encoded_bytes.decode('utf-8')
    engine.run_sign_test(
        path=PATH,
        tx_type=SIGNED_CODES.SOME_DATA,
        version=0,
        data_b58=encoded_str,
        num_clicks=3
    )

def test_long_data_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    sign_data = bytes("test data to sign " * 10, "utf-8")
    encoded_bytes = base58.b58encode(sign_data)
    encoded_str = encoded_bytes.decode('utf-8')
    engine.run_sign_test(
        path=PATH,
        tx_type=SIGNED_CODES.SOME_DATA,
        version=0,
        data_b58=encoded_str,
        num_clicks=3
    )    

def test_sign_long_request(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    sign_data = bytes("test data to sign " * 10, "utf-8")
    encoded_bytes = base58.b58encode(sign_data)
    encoded_str = encoded_bytes.decode('utf-8')
    engine.run_sign_test(
        path=PATH,
        tx_type=254,
        version=0,
        data_b58=encoded_str,
        num_clicks=3
    )    

def test_sign_long_bytes(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    sign_data = bytes("test data to sign " * 10, "utf-8")
    encoded_bytes = base58.b58encode(sign_data)
    encoded_str = encoded_bytes.decode('utf-8')
    engine.run_sign_test(
        path=PATH,
        tx_type=255,
        version=0,
        data_b58=encoded_str,
        num_clicks=3
    )    


