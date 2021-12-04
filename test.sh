#!/bin/bash

curl http://0.0.0.0:8080/status

curl --header "Content-Type: application/json" \
  --request POST \
  --data '{"from":"en", "to":"es", "text": "Hello world"}' \
  http://0.0.0.0:8080/v1/translate


curl --header "Content-Type: application/json" \
  --request POST \
  --data '{"from":"es", "to":"en", "text": "Hola Mundo"}' \
  http://0.0.0.0:8080/v1/translate