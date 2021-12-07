#!/bin/bash

curl http://0.0.0.0:8080/status

curl --header "Content-Type: application/json" \
  --request POST \
  --data '{"from":"en", "to":"es", "text": "Rocket is a web framework for Rust. If youd like, you can think of Rocket as being a more flexible, friendly medley of Rails, Flask, Bottle, and Yesod. We prefer to think of Rocket as something new. Rocket aims to be fast, easy, and flexible while offering guaranteed safety and security where it can. Importantly, Rocket also aims to be fun, and it accomplishes this by ensuring that you write as little code as needed to accomplish your task."}' \
  http://0.0.0.0:8080/v1/translate


curl --header "Content-Type: application/json" \
  --request POST \
  --data '{"from":"es", "to":"en", "text": "Hola Mundo"}' \
  http://0.0.0.0:8080/v1/translate