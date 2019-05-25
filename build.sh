#!/bin/bash

echo "Creating project file (.pri)"
echo "==========================="
./makepri.sh

echo "Cleaning up build directory"
echo "==========================="

rm -rf build
mkdir build
cd build

echo "Creating Makefile"
echo "==========================="
qmake ../DblpSearcher.pro

echo "Going to build project..."
echo "==========================="
make -j8
