import requests

url = 'http://0.0.0.0:8080'


def test_single():
    query = {"from": "es", "to": "en", "text": "Hola Mundo"}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert resp.json()['result'] == "Hello World"


def test_long():
    text = 'Rocket is a web framework for Rust. If youd like, you can think of Rocket as being a more flexible, friendly medley of Rails, Flask, Bottle, and Yesod. ' \
           'We prefer to think of Rocket as something new. Rocket aims to be fast, easy, and flexible while offering guaranteed safety and security where it can. ' \
           'Importantly, Rocket also aims to be fun, and it accomplishes this by ensuring that you write as little code as needed to accomplish your task.'
    query = {"from": "en", "to": "es", "text": text}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert len(resp.json()['result']) >= len(text) * 0.8


def test_pivot():
    query = {"from": "es", "to": "et", "text": "Hola Mundo"}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert resp.json()['result'] == 'Tere Maailm'


def test_unknown_pair():
    query = {"from": "es", "to": "xx", "text": "Hola Mundo"}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 400


def test_invalid_request():
    resp = requests.post(url + '/v1/translate', data="xxx")

    assert resp.status_code == 400
