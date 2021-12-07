# Translation service

## Running
1. Download models from https://github.com/mozilla/firefox-translations-models/tree/main/models/prod
2. unzip
3. git clone this repo
4. git submodule update --init --recursive
5. Point `MODELS_DIR=` in `docker-run.sh` to models directory (which contains directories like `enes`, `deen` etc.)
5. bash docker-run.sh
6. bash test.sh