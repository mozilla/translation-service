#!/bin/bash

version=0.3.0

mkdir models
cd models

mkdir esen
mkdir enet
mkdir enru
mkdir enbg

cd esen
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/esen/lex.50.50.esen.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/esen/model.esen.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/esen/vocab.esen.spm

cd ../enet
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enet/lex.50.50.enet.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enet/model.enet.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enet/vocab.eten.spm


cd ../enru
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enru/lex.50.50.enru.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enru/model.enru.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enru/vocab.enru.spm

cd ../enbg
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enbg/lex.50.50.enbg.s2t.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enbg/model.enbg.intgemm.alphas.bin
wget https://storage.googleapis.com/bergamot-models-sandbox/${version}/enbg/vocab.bgen.spm