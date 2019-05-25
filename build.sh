#!/bin/bash

echo "Creating project file (.pri)"

./makepri.sh

echo "==========================="
echo "Cleaning up build directory"


rm -rf build
mkdir build
cd build

echo "==========================="
echo "Creating Makefile"

qmake ../DblpSearcher.pro

echo "==========================="
echo "Going to build project..."

make -j8
