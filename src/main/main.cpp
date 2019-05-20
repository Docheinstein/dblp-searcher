#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/dblp_xml_parser.h"
#include "commons/log/loggable/loggable.h"
#include "commons/profiler/profiler.h"
#include "commons/globals/globals.h"
#include "main/args.h"
#include "main/main_gui.h"

STATIC_LOGGING(Main, true)

static const char * const HELP =
R"#(NAME
	dblp-searcher

SYNOPSIS
	dblp-searcher <MODE> <INDEX_FOLDER_PATH> <INDEX_BASE_NAME> [OPTIONS]...

DESCRIPTION
	Perform full-text searches over the dblp.xml dump of DBLP.
	This program can be launched in two different mode:
	1) Index creation: parses the dblp.xml and creates the index files from it
	2) Search:	open the GUI for perform searches over the previously created
				index files

INDEX MODE
	--index, -I <dblp_file_path> <index_folder_path> <index_base_name>
		Starts in index creation mode.
		Requires two additional arguments, the path where to place the index
		files and the base name to use for the index files.
		e.g. --index /tmp/dblp.xml /tmp/dblp-index/ indexname

SEARCH MODE
	--search, -S <index_folder_path> <index_base_name>
		Starts the GUI in search mode.
		Requires two additional arguments, the path where to load the index
		files and the base name of those.
		e.g. --search /tmp/dblp-index/ indexname

	OPTIONALS
		--xml, -X <dblp_xml_file>
		Use the original XML for show the original XML content of the query matches.
		Must obviously be the same file used for the indexing.
)#";

Arguments arguments;

static int startIndexMode() {
	Q_ASSERT(arguments.mode == DblpSearcherMode::Index);

	Indexer indexer(arguments.indexFolderPath, arguments.baseIndexName);
	DblpXmlParser parser(arguments.dblpXmlFilePath, indexer);
	parser.parse();

	return 0;
}

int main(int argc, char *argv[])
{
	PROF_FUNC_BEGIN

	if (argc < 2) {
		QUIT(HELP);
	}
	try {
		// Parse the arguments
		parseArguments(argc, argv);
		printArguments();

		if (arguments.mode == DblpSearcherMode::Index) {
			_ii("Starting in INDEX mode");
			startIndexMode();
		}
		else if (arguments.mode == DblpSearcherMode::Search) {
			_ii("Starting in SEARCH mode");
			startSearchMode();
		}
		else {
			_ww("Arguments parsing failed; please provide a valid mode.");
			QUIT(HELP);
		}
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
