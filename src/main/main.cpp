#include <QDebug>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QtConcurrentRun>
#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/xml_parser.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/resolver/query_resolver.h"
#include "dblp/irmodel/impl/ief/ir_model_ief.h"
#include "gui/splash/splash_window.h"
#include "gui/main/main_window.h"
#include "commons/globals/globals.h"


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

	ARGUMENTS
		--lazy-ief
			All the ief of every term in the vocabulary is usually loaded
			before start the application; this speed up the further queries
			but may require some time.
			If you wish to start the application faster, use this option
			(but queries resolving may be slower).

)#";

enum class Mode {
	Index,
	Search
};

typedef struct Arguments {
	Mode mode;
	QString dblpFilePath;
	QString indexFolderPath;
	QString baseIndexName;
	bool lazyIefs;

	int argc;
	char **argv;
} Arguments;

[[noreturn]] static void dblpSearcherAbort() {
	qCritical() << HELP;
	exit(-1);
}

static int doIndexing(Arguments args) {
	Q_ASSERT(args.mode == Mode::Index);

	Indexer indexer(args.indexFolderPath, args.baseIndexName);
	XmlParser parser(args.dblpFilePath, indexer);
	parser.parse();

	return 0;
}

static int doSearch(Arguments args) {
	Q_ASSERT(args.mode == Mode::Search);

	QGuiApplication guiApp(args.argc, args.argv);
	QQmlEngine engine;

	// Allocates the windows (splash and main)

	SplashWindow splashWindow(&engine);
	if (!splashWindow.create()) {
		qCritical() << "Error occurred while creating SplashWindow";
		exit(-1);
	}

	MainWindow mainWindow(&engine);
	if (!mainWindow.create()) {
		qCritical() << "Error occurred while creating MainWindow";
		exit(-1);
	}

	splashWindow.setVisible(true);

	// Load async

	QtConcurrent::run([&args, &splashWindow, &mainWindow]() {

		auto splashProgressor = [&splashWindow](double progress) {
			splashWindow.setProgress(progress);
		};

		// INDEX HANDLER

		IndexHandler handler(args.indexFolderPath, args.baseIndexName);

		// Connect to signals for detect progress
		QObject::connect(&handler, &IndexHandler::keysLoadStarted, [&splashWindow]() {
			splashWindow.setStatus("Loading index file: keys");
		});
		QObject::connect(&handler, &IndexHandler::keysLoadProgress, splashProgressor);

		QObject::connect(&handler, &IndexHandler::vocabularyLoadStarted, [&splashWindow]() {
			splashWindow.setStatus("Loading index file: vocabulary");
		});
		QObject::connect(&handler, &IndexHandler::vocabularyLoadProgress, splashProgressor);

		handler.load();

		// IR MODEL

		IRModelIef irmodel(&handler);

		QObject::connect(&irmodel, &IRModelIef::initStarted, [&splashWindow]() {
			splashWindow.setStatus("Initializing IR Model");
		});
		QObject::connect(&irmodel, &IRModelIef::initProgress, splashProgressor);

		irmodel.init(true);

		// QUERY RESOLVER (gratis)
		QueryResolver resolver(&irmodel);

		// Load of everything finished, show main window

		splashWindow.setVisible(false);
		mainWindow.setVisible(true);
	});

	return guiApp.exec();
}


static Arguments parseArguments(int argc, char *argv[]) {
	Arguments args;
	args.argc = argc;
	args.argv = argv;

	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];

		auto readNextParam = [argc, argv, &i](QString &str, QString failReason) {
			i++;
			if (i >= argc) {
				qCritical() << failReason;
				dblpSearcherAbort();
			}
			str = argv[i];
		};

		auto readModeParams = [&readNextParam, &args]() {
			readNextParam(args.indexFolderPath, "Missing mode parameter: index folder path");
			readNextParam(args.baseIndexName, "Missing mode parameter: index base name");
		};

		// Mode
		if (streq(arg, "--index") || streq(arg, "-I")) {
			args.mode = Mode::Index;
			readNextParam(args.dblpFilePath, "Missing mode parameter: dblp file path");
			readModeParams();
		}
		else if (streq(arg, "--search") || streq(arg, "-S")) {
			args.mode = Mode::Search;
			readModeParams();
		}

		// Other options
		// Lazy

		else if (streq(arg, "--lazy-ief")) {
			args.lazyIefs = true;
		}
	}

	return args;
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		dblpSearcherAbort();
	}
	try {
		// Parse the arguments
		Arguments args = parseArguments(argc, argv);
		if (args.mode == Mode::Index) {
			qInfo() << "Starting in INDEX mode";
			doIndexing(args);
		}
		else if (args.mode == Mode::Search) {
			qInfo() << "Starting in SEARCH mode";
			doSearch(args);
		}
		else {
			qWarning() << "Arguments parsing faile; please provide a valid mode.";
			dblpSearcherAbort();
		}
	} catch (const char * e) {
	  qCritical() << "Exception occured; aborting for the following reason:"
				  << endl << ">>" << e;
	  return 0;
	}
}
