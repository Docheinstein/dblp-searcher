#include <QCoreApplication>
#include <QtConcurrent>

#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/dblp_xml_parser.h"
#include "commons/log/loggable/loggable.h"
#include "commons/profiler/profiler.h"
#include "commons/globals/globals.h"
#include "main/args/args.h"
#include "main/search/main_search.h"
#include "main/index/main_index.h"

STATIC_LOGGING(Main, true)

static const char * const HELP =
R"#(NAME
	dblp-searcher

SYNOPSIS
	dblp-searcher <MODE> <INDEX_FOLDER_PATH> <INDEX_BASE_NAME> [OPTIONS]...

DESCRIPTION
	Performs full-text searches over the dblp.xml dump of DBLP.
	This program can be launched in two different mode:
	1) Index creation: parses the dblp.xml and creates the index files from it
	2) Search: open the GUI for perform searches over the previously created indexes

INDEX MODE
	--index, -I <dblp_file_path> <index_folder_path> <index_base_name>
		Starts the appliaction in index creation mode.
		Requires three arguments, the dblp file path, the path where to place
		the index files and the base name to use for these.
		e.g. --index /tmp/dblp.xml /tmp/dblp-index/ indexname

SEARCH MODE
	--search, -S <index_folder_path> <index_base_name>
		Starts the application in search mode (with the GUI).
		Requires three arguments, the path where to load the index
		files and the base name of these.
		e.g. --search /tmp/dblp-index/ indexname

	OPTIONALS
		--xml, -X <dblp_xml_file>
		Use the original XML for show the original XML content of the query matches.
		Must obviously be the same file used for the indexing.

SEARCH MODE LANGUAGE
	The following mini language is supported by the query resolver:

	f-t-s: ([element-field:] search-pattern)+
	search-pattern: term | "phrasal terms"
	element-field: publication-search | venue-search
	publication-search : publication-element[.publication-field]
	publication-element: publication | article | incollection | inproc | phThesis | masterThesis
	publication-field: author | title | year
	venue-search: venue[.venue-field]
	venue-field: title | publisher

	e.g.
	1. information retrieval
	2. "information retrieval"
	3. article: data science
	4. incollection.title: "database logic"
	5. article: science venue.title: springer)#";

Arguments arguments;

int main(int argc, char *argv[])
{
	PROF_FUNC_BEGIN

	if (argc < 2) {
		QUIT_SOFT(HELP);
	}
	try {
		// Parse the arguments
		parseArguments(argc, argv);
		printArguments();

		_ii("Hello, " APPLICATION_NAME " v. " << APPLICATION_VERSION << " there!");

		if (arguments.mode == DblpSearcherMode::Index) {
			_ii("Starting in INDEX mode");
			return startIndexMode();
		}

		if (arguments.mode == DblpSearcherMode::Search) {
			_ii("Starting in SEARCH mode");
			return startSearchMode();
		}

		_ww("Arguments parsing failed; please provide a valid mode");
		QUIT(HELP);

	} catch (const char * e) {
	  qCritical() << "Exception occured; aborting for the following reason:"
				  << endl << ">>" << e;
	  return 0;
	}

	PROF_FUNC_END

#if PROFILER
	profPrint();
#endif

}
