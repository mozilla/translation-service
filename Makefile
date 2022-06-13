#!make

.ONESHELL:
SHELL=/bin/bash

build-docker:
	docker build -t translation-service .

compile:
	bash scripts/compile.sh

download-models:
	bash scripts/download-test-models.sh

run:
	docker run --name translation-service -it -d --rm -v $$(pwd)/models:/models -p 8080:8080 -e PORT=8080 translation-service

debug:
	docker run --name translation-service -it --rm -v $$(pwd):/app -v $$(pwd)/models:/models -p 8080:8080 translation-service bash

call:
	curl --header "Content-Type: application/json" \
      --request POST \
      --data '{"from":"es", "to":"en", "text": "Hola Mundo"}' \
      http://0.0.0.0:8080/v1/translate

python-env:
	pip install pytest locust

test:
	pytest tests/integration

load-test-ui:
	locust -f tests/load/stress.py --host http://0.0.0.0:8080 --tags mixed

load-test:
	locust -f tests/load/stress.py --host http://0.0.0.0:8080 --headless --tags mixed --spawn-rate 1 --users 500

setup-models:
	git clone https://github.com/mozilla/firefox-translations-models
	rm -rf models
	mkdir models
	mv firefox-translations-models/models/dev/* models/
	mv firefox-translations-models/models/prod/* models/
	gunzip -r models/
	rm -rf firefox-translations-models
