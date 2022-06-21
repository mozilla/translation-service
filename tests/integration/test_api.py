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
    query = {"from": "en", "to": "et", "text": text}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert len(resp.json()['result']) >= len(text) * 0.8


def test_pivot():
    query = {"from": "es", "to": "et", "text": "Hola Mundo"}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert resp.json()['result'] == 'Tere Maailm'


def test_non_latin():
    query = {"from": "en", "to": "ru", "text": 'How are you?'}

    resp = requests.post(url + '/v1/translate', json=query)
    translation = resp.json()['result']

    assert resp.status_code == 200
    # behavior of this model can be non-deterministic, both translations are correct
    assert translation == 'Как дела?' or translation == 'Как у тебя дела?'


def test_different_shortlist():
    query = {"from": "en", "to": "bg", "text": 'Hello world'}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert resp.json()['result'] == 'Здравей свят'

def test_json_escaping():
    query = {"from": "es", "to": "en", "text": '"Hola Mundo"'}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 200
    assert resp.json()['result'] == '"Hello World"'


def test_unknown_pair():
    query = {"from": "es", "to": "xx", "text": "Hola Mundo"}

    resp = requests.post(url + '/v1/translate', json=query)

    assert resp.status_code == 400


def test_invalid_request():
    resp = requests.post(url + '/v1/translate', data="xxx")

    assert resp.status_code == 400


def test_heartbeat():
    resp = requests.get(url + '/__heartbeat__')

    assert resp.status_code == 200

def test_lbheartbeat():
    resp = requests.get(url + '/__lbheartbeat__')

    assert resp.status_code == 200