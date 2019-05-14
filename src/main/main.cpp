#include <QDebug>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QtConcurrentRun>
#include <gui/ui/window/splash/splash_window.h>
#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/xml_parser.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/resolver/query_resolver.h"
#include "dblp/irmodel/impl/ief/ir_model_ief.h"
#include "gui/ui/window/main/main_window.h"
#include "gui/engine/gui_engine.h"
#include "commons/globals/globals.h"
#include "main/tests.cpp"
#include "commons/profiler/profiler.h"

#define QML_REGISTER_BASE "org.docheinstein."

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
)#";

enum class Mode {
	Index,
	Search
};

class Arguments {
public:
	Mode mode;
	QString dblpFilePath;
	QString indexFolderPath;
	QString baseIndexName;

	int argc;
	char **argv;
};

[[noreturn]] static void dblpSearcherAbort() {
	qCritical() << HELP;
	exit(-1);
}

static int startIndexMode(Arguments args) {
	Q_ASSERT(args.mode == Mode::Index);

	Indexer indexer(args.indexFolderPath, args.baseIndexName);
	XmlParser parser(args.dblpFilePath, indexer);
	parser.parse();

	return 0;
}

static int startSearchMode(Arguments args) {
	Q_ASSERT(args.mode == Mode::Search);

	// Gui application (must be before everything else)
	QGuiApplication guiApp(args.argc, args.argv);

	// Global qml engine
	QQmlEngine *engine = new QQmlEngine;
	GuiEngine::instance(engine);

	// Custom types registration
	qmlRegisterType<MainWindow>(QML_REGISTER_BASE "MainWindow", 1, 0, "MainWindow");

	// Declared index stuff outside the QtConcurrent::run() scope
	IndexHandler *indexHandler;
	IRModelIef *irmodel;
	QueryResolver *queryResolver;

	// Allocates the windows (splash and main)

	SplashWindow splashWindow;
	if (!splashWindow.create()) {
		qCritical() << "Error occurred while creating SplashWindow";
		exit(-1);
	}

	MainWindow mainWindow;
	if (!mainWindow.create()) {
		qCritical() << "Error occurred while creating MainWindow";
		exit(-1);
	}

	splashWindow.setVisible(true);

	// Load async
	QtConcurrent::run([&args, &splashWindow, &mainWindow,
					  &indexHandler, &irmodel, &queryResolver]() {

		auto splashProgressor = [&splashWindow](double progress) {
			splashWindow.setProgress(progress);
		};

		// INDEX HANDLER

		indexHandler = new IndexHandler(args.indexFolderPath, args.baseIndexName);

		// Connect to signals for detect progress

		// Keys
		QObject::connect(indexHandler, &IndexHandler::keysLoadStarted, [&splashWindow]() {
			splashWindow.setStatus("Loading index file: keys");
		});
		QObject::connect(indexHandler, &IndexHandler::keysLoadProgress, splashProgressor);

		// Vocabulary
		QObject::connect(indexHandler, &IndexHandler::vocabularyLoadStarted, [&splashWindow]() {
			splashWindow.setStatus("Loading index file: vocabulary");
		});
		QObject::connect(indexHandler, &IndexHandler::vocabularyLoadProgress, splashProgressor);

		// Crossrefs & Articles journals
		// Do both on the same status
		QObject::connect(indexHandler, &IndexHandler::crossrefsLoadStarted, [&splashWindow]() {
			splashWindow.setStatus("Loading index file: crossrefs");
		});
		QObject::connect(indexHandler, &IndexHandler::crossrefsLoadProgress,
						 splashProgressor);

		indexHandler->load();

		// IR MODEL

		irmodel = new IRModelIef(*indexHandler);

		QObject::connect(irmodel, &IRModelIef::initStarted, [&splashWindow]() {
			splashWindow.setStatus("Initializing IR Model");
		});
		QObject::connect(irmodel, &IRModelIef::initProgress, splashProgressor);

		irmodel->init();

		// QUERY RESOLVER (gratis)
		queryResolver = new QueryResolver(irmodel);

		// Load of everything finished, show main window

		qDebug() << ">>> Going to HIDE splash and show main window";
		splashWindow.setVisible(false);

		// Set the resolver
		mainWindow.setResolver(queryResolver);
		mainWindow.setVisible(true);
	});

	return guiApp.exec();
}

#define LOG_TAG "main"
#define CAN_LOG true

static Arguments parseArguments(int argc, char *argv[]) {
	Arguments args;
	args.argc = argc;
	args.argv = argv;

	_tt("Ehy there!");

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
	}

	return args;
}

int main(int argc, char *argv[])
{
//	map_reduce_tests();
//	omp_tests();
//	insert_sort_tests();
	PROF_FUNC_BEGIN

	if (argc < 2) {
		dblpSearcherAbort();
	}
	try {
		// Parse the arguments
		Arguments args = parseArguments(argc, argv);
		if (args.mode == Mode::Index) {
			qInfo() << "Starting in INDEX mode";
			startIndexMode(args);
		}
		else if (args.mode == Mode::Search) {
			qInfo() << "Starting in SEARCH mode";
			startSearchMode(args);
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

	PROF_FUNC_END

#if PROFILER
	prof_print();
#endif

}
