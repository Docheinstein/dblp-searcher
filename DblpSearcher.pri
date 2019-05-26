######################################################################
# Automatically generated by qmake (3.0) dom mag 26 07:49:29 2019
######################################################################

TEMPLATE = app

# Input
HEADERS += src/commons/config/app_config.h \
           src/commons/config/config.h \
           src/commons/const/const.h \
           src/commons/globals/globals.h \
           src/commons/profiler/profiler.h \
           src/commons/util/util.h \
           src/gui/engine/gui_engine.h \
           src/main/args/args.h \
           src/main/index/main_index.h \
           src/main/search/main_search.h \
           src/main/tests/tests.h \
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
           src/gui/components/dblp_xml_line/gui_dblp_xml_line.h \
           src/gui/components/dblp_xml_lines/gui_dblp_xml_lines.h \
           src/gui/components/element_details/gui_element_details.h \
           src/gui/components/element_publication/gui_element_publication.h \
           src/gui/components/element_publications/gui_element_publications.h \
           src/gui/components/query_match/gui_query_match.h \
           src/gui/components/query_matches/gui_query_matches.h \
           src/gui/windows/main/gui_main_window.h \
           src/gui/windows/splash/gui_splash_window.h \
           src/commons/file/filestream/base/stream_file.h \
           src/commons/file/filestream/data/data_stream_file.h \
           src/commons/file/filestream/text/text_stream_file.h \
           src/dblp/index/models/entities/index_entities.h \
           src/dblp/index/models/match/index_match.h \
           src/dblp/index/models/post/index_post.h \
           src/dblp/index/models/reference/index_term_ref.h \
           src/dblp/index/models/term/index_term.h \
           src/dblp/irmodel/impl/ief/ir_model_ief.h \
           src/dblp/query/resolver/models/query_outcome.h \
           src/dblp/xml/models/element/dblp_xml_element.h \
           src/dblp/query/query/models/base/query_base_part.h \
           src/dblp/query/query/models/basic/query_basic_part.h \
           src/dblp/query/query/models/publication/query_publication_part.h \
           src/dblp/query/query/models/types/query_types.h \
           src/dblp/query/query/models/venue/query_venue_part.h
SOURCES += src/main/main.cpp \
           src/commons/globals/globals.cpp \
           src/commons/profiler/profiler.cpp \
           src/commons/util/util.cpp \
           src/gui/engine/gui_engine.cpp \
           src/main/args/args.cpp \
           src/main/index/main_index.cpp \
           src/main/search/main_search.cpp \
           src/main/tests/tests.cpp \
           src/commons/adt/type/string_type.cpp \
           src/commons/log/loggable/loggable.cpp \
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
           src/gui/components/dblp_xml_line/gui_dblp_xml_line.cpp \
           src/gui/components/dblp_xml_lines/gui_dblp_xml_lines.cpp \
           src/gui/components/element_details/gui_element_details.cpp \
           src/gui/components/element_publication/gui_element_publication.cpp \
           src/gui/components/element_publications/gui_element_publications.cpp \
           src/gui/components/query_match/gui_query_match.cpp \
           src/gui/components/query_matches/gui_query_matches.cpp \
           src/gui/windows/main/gui_main_window.cpp \
           src/gui/windows/splash/gui_splash_window.cpp \
           src/commons/file/filestream/base/stream_file.cpp \
           src/commons/file/filestream/data/data_stream_file.cpp \
           src/commons/file/filestream/text/text_stream_file.cpp \
           src/dblp/index/models/entities/index_entities.cpp \
           src/dblp/index/models/match/index_match.cpp \
           src/dblp/index/models/post/index_post.cpp \
           src/dblp/index/models/reference/index_term_ref.cpp \
           src/dblp/irmodel/impl/ief/ir_model_ief.cpp \
           src/dblp/xml/models/element/dblp_xml_element.cpp \
           src/dblp/query/query/models/base/query_base_part.cpp \
           src/dblp/query/query/models/basic/query_basic_part.cpp \
           src/dblp/query/query/models/publication/query_publication_part.cpp \
           src/dblp/query/query/models/types/query_types.cpp \
           src/dblp/query/query/models/venue/query_venue_part.cpp
