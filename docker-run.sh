#!/bin/bash
MODELS_DIR=

docker build -t translation-server .
# docker run --name translation-server -it --rm -v $(pwd):/app -v $MODELS_DIR:/models -p 8080:8080 translation-server bash
docker run --name translation-server -it --rm -v $MODELS_DIR:/models -p 8080:8080 translation-server