#!/bin/bash
qmake -project -nopwd -o DblpSearcher.pri $PWD/src/
grep -v INCLUDEPATH DblpSearcher.pri > DblpSearcher.pri.new
mv DblpSearcher.pri.new DblpSearcher.pri
