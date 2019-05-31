#!/bin/bash

echo "Creating project file (.pri)"

./makepri.sh

echo "==========================="
echo "Eventually creating build folder"

mkdir -p build
cd build

echo "==========================="
echo "Generating Makefile"

qmake ../DblpSearcher.pro

echo "==========================="
echo "Going to build project..."

make -j8
