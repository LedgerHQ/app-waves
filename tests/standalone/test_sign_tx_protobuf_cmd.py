from application_client.waves_sign_data import WavesSignData, WavesTestEngine

PATH = "m/44'/5741564'/0'/0'/1'"
CHAIN_ID = ord("W") 

transferTxV3 = "3FLRBDDq1tWjEqJBuSrfiRgADthiQxuM5PycQcRNGV66FUwy7zwGYy9JFTrruSF3dHs1vrpAbJDup7sVtaWqqNyht3HKpqhfq3SKngp681TMmcWnPWqfokyUULhuUqJWEEoegJ8WVBnonMiC6GnJRFdjdUuWRK2Xx7axz8k1LSyGTZVUed3oxcDB69z8MBj1dLqQfZjMJUkdkXjhQw8zi"
issureTxV3 = "SRsmPviNi2vGykWmRk8bGcPksfMpSnhDwiFmk56yAe6SBdcdoSNmZvnENeXDjD3ESHpiSqrZwAz3ci4mjsooRFchTrq9MseyicvnUj7dsetCx1DPgeCG5yzKixMpi"
reissureTxV3 = "DgUjnJNJ1Y1TCcy8yGauWFyRoUsReXiWqU5nY5zEen5TqVw1NQ59ea2pMZGbgH58TgLuVrXqrabAYacVkedQ6deBLgbkrY7XfNJvRp9J47pvwwwmy9aysqDjkySAcR3CtQ2c"
burnTxV3 = "2wPxLeU3495Qzi4TXVLYj4bj53ynjZHDfeubBDL38RNMh6meg8C8bsFJizep9r42UFQUoU2321k8MMMug9tqAKAWJf7CyTbf8d32GqaHh9WjaXyXwKpCmEvjZbHoGWq"
leaseTxV3 = "bVaAp1EtxsEUK85gF5c383guJ4pbVCHHbhJrbFL6gJzXDs18CcueXGEstWbVA2rrpDmahGCZk8KwdkhYbAVzE6qfKAF88brdoPYaaTzKHbd8WnntQT"
cancelLeaseTxV3 = "4ti39jgD6j3gtxhGyrrmduPPcGVnD69pRnkTzJQoXuSbJBTGuiUN9R8q6CwaeBsEAiUjnj9aQBgGiB7KTt4oCke5yXCFKQFmrpcsDnSRHDdSdund8hEVZ9At"
createAliasTxV3 = "4DKMKpxxQTdhory85DjQ8ewVgAcXJqvpm9hHJqnrfCyALTAirv8U81XUphQSG4L1yaVUnwo7Gxz74kNNGp"
massTransferTxV3 = "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"
dataTxV3 = "2QQvuvG9rZLCh3XmrydJWuJvbc7c7u2NRo6HFUnkQ4rZ8z9Du9qrUH8tce95g3VbPyUcPZKF2ZKwmB2dXN2EJ4z5io2yMJEpYrNo6ZWzmeZRswAgSU1BDbmzqvYvVaew2rzWPjnR5tAX23kSPqAwC9CdfCvggKNfqNPQdMrkJQWusgEyzt49iMmjoCPowMpNCbPaupBAWWFnKJWxSDPHwno6CuhatwyDzqNQSjQT5vm2jV6aE9ykTUVkHTQPPD1fYM4yrLonaYN5Yzr76De3smWywMEVZE5XMZABCj6gt38v7HbHgVGR4TK5HoqeccMwiTzx3tpR3krprJtE5qWiA9V2g2uUAfQnEanPLAzmvdu1iyo5YL9NfdLRKq4ophJreYvvSwz9B2sXrbTCihgoy6WxAFHvdGS7fx9LQSWW8Rav6orghdDSuDqA5jP3K8SDYoJp8PBVAxMQU3xXWVT7JgpfVQrqQj1mVuF6chU9gTf93E4tLiFxs9YbGKXPbaxpb9So9Zx3uKGnD5DoNBE9tSpBZHc2DRwwdqBpEcXoyJ6jXLhfcj7dx1xJnAdC3UjEFP6PtAAJfosdxVxLTKC9NVj3mtfSz5cj83wDe1VUL9N5mX5imbHDn6XwctkSFsEuysq9bFT9Z3xRZiW5iTo8R2TMwP4siPnyidCQoTjDCgv7RDAVsKAE7fNno7hFUo7tHvGCm8EhnbzsMFrBNjSVfNK6EBhYuLxwZGLmYci1GpbuNGWkq8sekWCQFtTVZN6GQmVzYxhTsrCtoBb3BkiU3o5U9YamuZ6JkEi2Csm3jiKfVbMTraGyjR67uY5ZVAaNJ16UosqcV5HD6egND3CDD5ujrMEPpte5rTs7w6D8HGikYGLeoWBqw4FnfhzSCDcLCQAJpsJ7GjbWyHJ7Hkv1mkcJoyCqT1j9mNydweoGsrs2F3YHDt5cgPxWzyFdKgJ7wG8RBBXzJjx2ZzNxYxA8arMNCRet4LYUzEVWYEn1o2GVeG3CeSeMXYbrkRHYcka9gyqt4cr6WJJQmkwa4dJzPQGybuTAConErpWsEPMrPfcwsVuSsdFmspamaAcH7Du66Jqs1r29pvWhqW3LqSuLNui9HJWzQ9x33RdHg2xXzVv1BCjKDfrjXNEsXhFebLQwn7PEgVwQkx1QjygFgVCVi2F8RsUCYdUEi3UeYMQSXZKbKs9aS9AM9kE5KFNGgrYmDc3WQYFehrcgd2e8sB1VtGe771sBPFKViMJBBrEL1WCxBrmxfQkoqwSYabr8ZZuN1Vv8zzUvtwf27rqKTghrULspAAEU3K3VLCUfbuim6m56yAUaYpqVL6MzdjA684aNNA27AHMiPGcJz6vgAQFrPUDkChCFXPWQTZ1ws8cskyF9q1y8ZoZaCHdiK22DiVsakawWCBMgA76wfqvN2HoqqdX6fes5a82KW8Y3xHYkyG88Ndj4j4pwEqiV5siYVkLCStqYvBEnHxPDecHJeddajWLyTgUoBrmzsYL7xnDKhFKTwj9r1HXSYZ92UvKtnfp3v8YMrjM1vzVabs3Kpe9J6FcG4ansgCtteZSXu9NawANyJinQJX36EUADJjHw3cgUBPHq4zZdQSYxMHH8qTStPTbmm8yL8c7xyAkYU2UzCasHgq8f35Sgd2xheXuKNQmJwsiqmWG6tBLZ6aXReaBXaTTXDQWaG3h8bkQJ9DYwBzCucUUZYfpvhjpFMfNjXEW6braJMtihygJZTPXyyR8ED5MS7mDBZtuT1tXCP8skAuhs45zG8zoU7vNeeM5yuegZrPv3GX5ZwnLnuqNnDMXBwaHJXk2cT7jnfyyhTQUvY5K8FqkdkWYn8zux"
setScriptTxV3 = "4eVNF6daEEByWtmmKqaMxvaJyKqSXtnPsEWDJ8xgxCJN7EisovETyPgTYtcgsimr5nE45jqsmnMrcLWbjoHK59GQhF4w22JydUSwieYuUL5ShXh7xBSaJWXYoFesEtkGdh8NonvHLjejj2fZxtd2gmwZM6TQMvLqyQq3AkT9AZLBpgudrfvfUjqdJDtBY4wHtRfNXorZhN39LE9WoQmWgYzbXk4mPmkWFbbRYsLC"
sponsorFeeTxV2 = "DYFkvHUWAYo6FYoPDjLUZSX5b2PK2NkNydizo3YMDjqD4gEE1LvBPrtm1Zcy2p2HEEhj2HmvPNRW9WdvsLfzBXwjy4cTb4zAHWMGxtQZwER4KdsDVykKMkZRTeRP"
setAssetScriptTxV2 = "DgUjn8iZDiGTjnprkXbeAa15JGyQtNR6QrKyNba5ijG5BFL7pZNYTsg27DAcUPZYQ5RK8dggz9W2MPLrLSR5C14KwHQudZATcmGr6R9WQgWS8ujf6SD7sxnV9zF976ZDvnp6"
invokeTxV2 = "32Meu5TUM6tE17u3P77Gq1i29kKXXQ9Q6V4ymX3VFh7cZGJgp4S6M6qtGguBaKie8FyeBno6xKw6DPWKmbXx5rQ3ZiPK3JXGWz3nWeVYLDHY8sRq7KPk17CL1TaFLtoJsCJME1QxTMGBye1nvyg39Q2EqmqSL8DwjV4346G6MsJGH3H6DVJzTtMfPxf6mGCd2zXAFmWjbN6pD4xThgrawihP9QskW1Z3MXQoik2nSJsRGGz67jurNeu9GmdgeUrGUzBX1DMdYSwhYPjDNCTWD3w1DZxUUQ98sWAiapapwBfqoxRQGRk4GgQCfEBt9ef29pQoPEJhgzgnXYxJocowE6kFeb4rEFJ8tLdU45YgtvxN4CoWSTWUsrX8ULxHquUy9KX5EXwq8L3DTTCx8sEYckCE9BDnrecmCsi12pdjpM2EwVpotFDRmrznSNY1bJkP5Aed5JUf6SV3tcAYhinYW5vYUBBX95Tb6wsYaiHEtpmzFvetynTBBvtEjWT1x7GqUJThF6quYt1XJJoos2Rv4bnkZcPmj1Z1Bj9hsdDShWCZscysPyyk"
updateAssetTxV1 = "ALxgMhjBhXR1pa6Svsv3cKAoNNxo3FtbAh1BoysjPdKwqdrmFqXi5coeWjguD8uNNqrymCxgSrFyoDa2zjGLkqkB9nSS1qtK81sfarxgKMkLugLCsLP4iEbE3wLApKPpH6hHxgKgNMwhwm9"

def test_data_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=12,
        version=3,
        data_b58=dataTxV3,
        num_clicks=5
    )

def test_update_asset_v1_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=17,
        version=2,
        data_b58=updateAssetTxV1,
        num_clicks=8
    )

def test_invoke_v2_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=16,
        version=2,
        data_b58=invokeTxV2,
        num_clicks=11
    )


def test_set_asset_script_v2_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=15,
        version=2,
        data_b58=setAssetScriptTxV2,
        num_clicks=6
    )


def test_sponsor_fee_v2_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=14,
        version=2,
        data_b58=sponsorFeeTxV2,
        num_clicks=7
    )


def test_set_script_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=13,
        version=3,
        data_b58=setScriptTxV3,
        num_clicks=5
    )


def test_mass_transfer_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=11,
        version=3,
        data_b58=massTransferTxV3,
        num_clicks=7
    )


def test_cancel_lease_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=9,
        version=3,
        data_b58=cancelLeaseTxV3,
        num_clicks=6
    )

def test_lease_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=8,
        version=3,
        data_b58=leaseTxV3,
        num_clicks=6
    )

def test_create_burn_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=6,
        version=3,
        data_b58=burnTxV3,
        num_clicks=7
    )

def test_create_alias_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=10,
        version=3,
        data_b58=createAliasTxV3,
        num_clicks=5
    )


def test_reissure_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=5,
        version=3,
        data_b58=reissureTxV3,
        num_clicks=8
    )

def test_issure_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=3,
        version=3,
        data_b58=issureTxV3,
        num_clicks=10
    )

def test_transfer_v3_signing(backend, navigator, test_name):    
    engine = WavesTestEngine(backend, navigator, WavesSignData(backend))
    
    engine.run_sign_test(
        path=PATH,
        tx_type=4,
        version=3,
        data_b58=transferTxV3,
        num_clicks=9
    )