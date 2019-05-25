#!/bin/bash

BASEDIR=$(dirname "$0")

qmake -project -nopwd -o - $BASEDIR/src/ > $BASEDIR/DblpSearcher.pri
grep -v TARGET $BASEDIR/DblpSearcher.pri > $BASEDIR/DblpSearcher.pri.new
mv $BASEDIR/DblpSearcher.pri.new $BASEDIR/DblpSearcher.pri
