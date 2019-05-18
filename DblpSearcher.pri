######################################################################
# Automatically generated by qmake (3.1) Sat May 18 12:26:48 2019
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
HEADERS += src/main/args.h \
           src/commons/config/config.h \
           src/commons/const/const.h \
           src/commons/globals/globals.h \
           src/commons/profiler/profiler.h \
           src/commons/util/util.h \
           src/gui/engine/gui_engine.h \
           src/gui/main/gui_main_window.h \
           src/gui/splash/gui_splash_window.h \
           src/commons/adt/type/string_type.h \
           src/commons/adt/type/type.h \
           src/commons/log/loggable/loggable.h \
           src/commons/log/logger/logger.h \
           src/dblp/index/handler/index_handler.h \
           src/dblp/index/indexer/indexer.h \
           src/dblp/irmodel/base/ir_model.h \
           src/dblp/query/match/query_match.h \
           src/dblp/query/query/query.h \
           src/dblp/query/resolver/query_resolver.h \
           src/dblp/shared/defs/defs.h \
           src/dblp/shared/element_field_type/element_field_type.h \
           src/dblp/shared/element_type/element_type.h \
           src/dblp/xml/handler/dblp_xml_parse_handler.h \
           src/dblp/xml/parser/dblp_xml_parser.h \
           src/gui/base/component/gui_component.h \
           src/gui/models/dblp_xml_line/gui_dblp_xml_line.h \
           src/gui/models/dblp_xml_lines/gui_dblp_xml_lines.h \
           src/gui/models/element_details/gui_element_details.h \
           src/gui/models/element_publication/gui_element_publication.h \
           src/gui/models/element_publications/gui_element_publications.h \
           src/gui/models/query_match/gui_query_match.h \
           src/gui/models/query_matches/gui_query_matches.h \
           src/commons/file/filestream/base/stream_file.h \
           src/commons/file/filestream/data/data_stream_file.h \
           src/commons/file/filestream/text/text_stream_file.h \
           src/dblp/index/models/entities/index_entities.h \
           src/dblp/index/models/key/index_key.h \
           src/dblp/index/models/match/index_match.h \
           src/dblp/index/models/post/index_post.h \
           src/dblp/index/models/reference/index_term_ref.h \
           src/dblp/index/models/term/index_term.h \
           src/dblp/irmodel/impl/ief/ir_model_ief.h \
           src/dblp/xml/models/element/dblp_xml_element.h \
           src/dblp/xml/models/types/dblp_xml_types.h \
           src/dblp/query/query/models/base/query_base_part.h \
           src/dblp/query/query/models/basic/query_basic_part.h \
           src/dblp/query/query/models/publication/query_publication_part.h \
           src/dblp/query/query/models/types/query_types.h \
           src/dblp/query/query/models/venue/query_venue_part.h
SOURCES += src/main/args.cpp \
           src/main/main.cpp \
           src/main/tests.cpp \
           src/commons/globals/globals.cpp \
           src/commons/profiler/profiler.cpp \
           src/commons/util/util.cpp \
           src/gui/engine/gui_engine.cpp \
           src/gui/main/gui_main_window.cpp \
           src/gui/splash/gui_splash_window.cpp \
           src/commons/adt/type/string_type.cpp \
           src/commons/log/loggable/loggable.cpp \
           src/commons/log/logger/logger.cpp \
           src/dblp/index/handler/index_handler.cpp \
           src/dblp/index/indexer/indexer.cpp \
           src/dblp/irmodel/base/ir_model.cpp \
           src/dblp/query/match/query_match.cpp \
           src/dblp/query/query/query.cpp \
           src/dblp/query/resolver/query_resolver.cpp \
           src/dblp/shared/element_field_type/element_field_type.cpp \
           src/dblp/shared/element_type/element_type.cpp \
           src/dblp/xml/handler/dblp_xml_parse_handler.cpp \
           src/dblp/xml/parser/dblp_xml_parser.cpp \
           src/gui/base/component/gui_component.cpp \
           src/gui/models/dblp_xml_line/gui_dblp_xml_line.cpp \
           src/gui/models/dblp_xml_lines/gui_dblp_xml_lines.cpp \
           src/gui/models/element_details/gui_element_details.cpp \
           src/gui/models/element_publication/gui_element_publication.cpp \
           src/gui/models/element_publications/gui_element_publications.cpp \
           src/gui/models/query_match/gui_query_match.cpp \
           src/gui/models/query_matches/gui_query_matches.cpp \
           src/commons/file/filestream/base/stream_file.cpp \
           src/commons/file/filestream/data/data_stream_file.cpp \
           src/commons/file/filestream/text/text_stream_file.cpp \
           src/dblp/index/models/entities/index_entities.cpp \
           src/dblp/index/models/match/index_match.cpp \
           src/dblp/index/models/post/index_post.cpp \
           src/dblp/index/models/reference/index_term_ref.cpp \
           src/dblp/irmodel/impl/ief/ir_model_ief.cpp \
           src/dblp/xml/models/element/dblp_xml_element.cpp \
           src/dblp/xml/models/types/dblp_xml_types.cpp \
           src/dblp/query/query/models/base/query_base_part.cpp \
           src/dblp/query/query/models/basic/query_basic_part.cpp \
           src/dblp/query/query/models/publication/query_publication_part.cpp \
           src/dblp/query/query/models/types/query_types.cpp \
           src/dblp/query/query/models/venue/query_venue_part.cpp
