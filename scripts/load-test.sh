#!/bin/bash

# pip install locust

locust -f tests/load/stress.py --host http://0.0.0.0:8080 --tags mixed
# or run without UI
#locust -f tests/load/stress.py --host http://0.0.0.0:8080 --headless --tags mixed --spawn-rate 1 --users 500