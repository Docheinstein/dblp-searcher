######################################################################
# Automatically generated by qmake (3.1) Wed May 1 10:01:14 2019
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
           debris/type/string_type.h \
           debris/type/type.h \
           src/main/mainwindow.h \
           src/commons/const/const.h \
           src/commons/logger/logger.h \
           src/commons/shared/shared.h \
           src/commons/util/util.h \
           src/commons/adt/type/string_type.h \
           src/commons/adt/type/type.h \
           src/dblp/index/handler/index_handler.h \
           src/dblp/index/indexer/indexer.h \
           src/dblp/query/parser/query_parser.h \
           src/dblp/xml/handler/xml_parse_handler.h \
           src/dblp/xml/parser/xml_parser.h \
           src/dblp/index/models/fieldtypes/index_element_field_type.h \
           src/dblp/index/models/key/index_key.h \
           src/dblp/index/models/post/index_post.h \
           src/dblp/index/models/reference/index_term_ref.h \
           src/dblp/index/models/term/index_term.h \
           src/dblp/query/models/base/query_base_part.h \
           src/dblp/query/models/basic/query_basic_part.h \
           src/dblp/query/models/publication/query_publication_part.h \
           src/dblp/query/models/query/query.h \
           src/dblp/query/models/types/query_types.h \
           src/dblp/query/models/venue/query_venue_part.h \
           src/dblp/xml/models/entities/xml_entities.h \
           src/dblp/xml/models/types/xml_types.h
FORMS += src/main/mainwindow.ui
SOURCES += src/main/main.cpp \
           src/main/mainwindow.cpp \
           src/commons/logger/logger.cpp \
           src/commons/util/util.cpp \
           src/commons/adt/type/string_type.cpp \
           src/dblp/index/handler/index_handler.cpp \
           src/dblp/index/indexer/indexer.cpp \
           src/dblp/query/parser/query_parser.cpp \
           src/dblp/xml/handler/xml_parse_handler.cpp \
           src/dblp/xml/parser/xml_parser.cpp \
           src/dblp/index/models/fieldtypes/index_element_field_type.cpp \
           src/dblp/index/models/reference/index_term_ref.cpp \
           src/dblp/query/models/base/query_base_part.cpp \
           src/dblp/query/models/basic/query_basic_part.cpp \
           src/dblp/query/models/publication/query_publication_part.cpp \
           src/dblp/query/models/query/query.cpp \
           src/dblp/query/models/types/query_types.cpp \
           src/dblp/query/models/venue/query_venue_part.cpp \
           src/dblp/xml/models/entities/xml_entities.cpp \
           src/dblp/xml/models/types/xml_types.cpp
