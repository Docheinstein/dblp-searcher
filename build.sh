#!/bin/bash

cd build

echo "==========================="
echo "Creating Makefile"

qmake ../DblpSearcher.pro

echo "==========================="
echo "Going to build project..."

make -j8
