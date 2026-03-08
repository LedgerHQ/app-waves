
from application_client.waves_sign_data import WavesSignData, WavesTestEngine


PATH = "m/44'/5741564'/0'/0'/1'"
CHAIN_ID = ord("W") 
ORDER_V4_B58 = "8sK1LUwKeq3Xa37CxgBnT3pZHZWekH9ba2Pv4CWhcuT2wuiFuWpe9M5ED4pGocfk5M5mSM9oES3NWFVHVEvzSRwczjqBWcV1Lazz6PdSDcnbYTDjPS7e9TvPGHpt4xmvYz8W8XUjHiHZpb5FxqT97gZm6sKe3skZz7mUzw4WzkiUFshWgNqcqSL6H1bCpRLDGm6cMukQnWtyr9WbhkyqjLfP6afz3JvZY6MfJJEdkWidBk31RGRtaBegyFQCVFNGfUexSFFHdzmrLTgNRQLqUy8UDJHVmTwjBKmYgjZ1hqTSChsL3oo3Ah7nLDGFD9EFs1uWHkvk9xo1Ph41KzaVK4LyMAyh7w93vipFvGk8oKn8fnHCHN8KQUwv9G4hJuvHqtRKiYhpLPGEFwNfgyFw6G5g3Y3PLT1"
ORDER_V3_B58 = "8sK1LUwKeq3Xa37CxgBnT3pZHZWekH9ba2Pv4CWhcuT2wuiFuWpe9M5ED4pGocfk5M5mSM9oES3NWFVHVEvzSRwczjqBWcV1Lazz6PdSDcnbYTDjPS7e9TvPGHpt4xmvYz8W8XUjHiHZpb5FxqT97gZm6sKe3skZz7mUzw4WzkiUFshWgNqcqSL6H1bCpRLDGm6cMukQnWtyr9WbhkyqjLfP6afz3JvZY6MfJJEdkWffmK4ZnN5QrS5PE24iUj8mFB38yoaACeych1saRs2pecoQ4odZxaSNxVMkw8wuhZ5RSN37s1yfeGUdr7cocEpmx2nYCospqmUaW1C87JD4uAjsDisMoM7oHpDpHsdrt3LgBKkoxJgwHoP8y2Jo4MDEZWeSHYP2hg3ZZqw1imDwfb2KWizfdA7"

def test_order_v4_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=252,
        version=4,
        data_b58=ORDER_V4_B58,
        num_clicks=9
    )

def test_order_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=252,
        version=3,
        data_b58=ORDER_V3_B58,
        num_clicks=3
    )    