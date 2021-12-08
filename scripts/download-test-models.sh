#!/bin/bash

mkdir models
cd models

mkdir esen
mkdir enet

cd esen
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/esen/lex.50.50.esen.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/esen/model.esen.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/esen/vocab.esen.spm

cd ../enet
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/enet/lex.50.50.enet.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/enet/model.enet.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.2.10/enet/vocab.eten.spm