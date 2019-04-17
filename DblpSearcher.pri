######################################################################
# Automatically generated by qmake (3.1) Wed Apr 17 16:47:59 2019
######################################################################

TEMPLATE = app
TARGET = DblpSearcher

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += build/ui_mainwindow.h \
           build-dbg/ui_mainwindow.h \
           src/main/mainwindow.h \
           src/commons/const/const.h \
           src/commons/logger/logger.h \
           src/commons/shared/shared.h \
           src/commons/util/util.h \
           src/dblp/entities/dblp_entities.h \
           src/dblp/index/handler/handler.h \
           src/dblp/index/indexer/indexer.h \
           src/dblp/index/key/index_key.h \
           src/dblp/index/post/index_post.h \
           src/dblp/index/reference/index_term_ref.h \
           src/dblp/index/term/index_term.h \
           src/dblp/parse/handler/dblp_xml_parse_handler.h \
           src/dblp/parse/parser/dblp_xml_parser.h
FORMS += src/main/mainwindow.ui
SOURCES += src/main/main.cpp \
           src/main/mainwindow.cpp \
           src/commons/logger/logger.cpp \
           src/commons/util/util.cpp \
           src/dblp/entities/dblp_entities.cpp \
           src/dblp/index/handler/handler.cpp \
           src/dblp/index/indexer/indexer.cpp \
           src/dblp/index/reference/index_term_ref.cpp \
           src/dblp/parse/handler/dblp_xml_parse_handler.cpp \
           src/dblp/parse/parser/dblp_xml_parser.cpp
