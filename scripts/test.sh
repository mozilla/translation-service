#!/bin/bash

curl --header "Content-Type: application/json" \
  --request POST \
  --data '{"from":"en", "to":"es", "text": "Hello World!"}' \
  http://0.0.0.0:8080/v1/translate