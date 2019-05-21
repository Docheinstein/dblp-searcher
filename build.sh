#!/bin/bash

rm -rf build
mkdir build
cd build
../makepri.sh
qmake ../DblpSearcher.pro
make -j8
