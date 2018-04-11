#!/bin/bash
rm -rf ./toalex/*
cp ./src/*.cpp ./toalex/
cp ./src/*.h ./toalex/
cp ./calls-log-result-export.csv ./toalex/
cp ./README.txt ./toalex/
cp ./run_prediction.py ./toalex/
tar -zcvf ToAlex.tar.gz ./toalex/
