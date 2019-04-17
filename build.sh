#!/bin/bash
qmake -project -o DblpSearcher.pri
grep -v INCLUDEPATH DblpSearcher.pri > DblpSearcher.pri.new
mv DblpSearcher.pri.new DblpSearcher.pri
