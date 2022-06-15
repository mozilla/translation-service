#!/bin/bash

mkdir models
cd models

mkdir esen
mkdir enet
mkdir enru
mkdir enuk

cd esen
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/esen/lex.50.50.esen.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/esen/model.esen.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/esen/vocab.esen.spm

cd ../enet
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enet/lex.50.50.enet.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enet/model.enet.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enet/vocab.eten.spm


cd ../enru
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enru/lex.50.50.enru.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enru/model.enru.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enru/vocab.enru.spm

cd ../enuk
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enuk/lex.50.50.enuk.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enuk/model.enuk.intgemm8.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enuk/srcvocab.enuk.spm
wget https://storage.googleapis.com/bergamot-models-sandbox/0.3.0/enuk/trgvocab.enuk.spm