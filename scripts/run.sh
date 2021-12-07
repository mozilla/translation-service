#!/bin/bash
# docker build -t translation-service .
# docker run --name translation-service -it --rm -v $(pwd):/app -v $MODELS_DIR:/models -p 8080:8080 translation-service bash
docker run --name translation-service -it --rm -v $MODELS_DIR:/models -p 8080:8080 translation-service