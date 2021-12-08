# Translation service

HTTP service that uses [bergamot-translator](https://github.com/mozilla/bergamot-translator) and compressed neural machine translation [models](https://github.com/mozilla/firefox-translations-models) for fast inference on CPU.

## Running locally
1. git clone this repo
2. Download models from https://github.com/mozilla/firefox-translations-models/tree/main/models/prod, unzip and copy to `models` directory or use `make models`
3. `make build-docker`
4. `make run`
5. `make call`

## Service configuration

Directory that contains models ('esen', 'ende' etc.) should be mounted to `/models` in Docker container.

Environment variables to set in container:

`PORT` - service port (default is 8000)

`LOGGING_LEVEL` - ERROR, WARNING, INFO or DEBUG (default is INFO)

`WORKERS` - number of bergamot-translator workers (default is 1). 0 - automatically set as number of available CPUs.
It is recommended to minimize workers and scale horizontaly with k8s means.


## Testing

`make python-env` - install pip packages

`make test` - to run integration API tests

`make load-test` - to run a stress test (requires more models to download that unit tests)
