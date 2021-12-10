# Translation service

HTTP service that uses [bergamot-translator](https://github.com/mozilla/bergamot-translator) and compressed neural machine translation [models](https://github.com/mozilla/firefox-translations-models) for fast inference on CPU.

## Running locally
1. git clone this repo
2. `make setup-models`
3. `make build-docker`
4. `make run`
5. `make call`

## Calling the service
````
$ curl --header "Content-Type: application/json" \
      --request POST \
      --data '{"from":"es", "to":"en", "text": "Hola Mundo"}' \
      http://0.0.0.0:8080/v1/translate
> {"result": "Hello World"}
`````

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
