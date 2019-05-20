#include <QDebug>
#include <QGuiApplication>
#include <QQmlEngine>
#include <QtConcurrentRun>
//#include <gui/ui/window/splash/splash_window.h>
#include "dblp/index/indexer/indexer.h"
//#include "dblp/xml/parser/xml_parser.h"
//#include "dblp/xml/parser2/xml_parser2.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/resolver/query_resolver.h"
#include "dblp/irmodel/impl/ief/ir_model_ief.h"
//#include "gui/ui/window/main/main_window.h"
#include "gui/engine/gui_engine.h"
#include "commons/globals/globals.h"
#include "main/tests.h"
#include "commons/profiler/profiler.h"
#include "gui/splash/gui_splash_window.h"
#include "gui/main/gui_main_window.h"
#include "gui/models/element_details/gui_element_details.h"
//#include "gui/resolver/gui_query_resolver.h"
#include <QQmlComponent>
#include "dblp/xml/models/element/dblp_xml_element.h"
#include "dblp/xml/handler/dblp_xml_parse_handler.h"
#include "dblp/xml/parser/dblp_xml_parser.h"
#include "commons/log/loggable/loggable.h"
#include "main/args.h"
#include "main_gui.h"

//#define QML_URI				"DblpSearcher"
//#define QML_VERSION_MAJOR	1
//#define QML_REVISION		0

#define QML_REGISTRAR QML_URI, QML_VERSION_MAJOR, QML_REVISION

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
		Use the original XML for show the original content of the query matches.
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

//static int startSearchMode() {
//	Q_ASSERT(arguments.mode == DblpSearcherMode::Search);

//	// Gui application (must be before everything else)
//	QGuiApplication guiApp(arguments.argc, arguments.argv);

//	// Global qml engine
//	QQmlEngine *engine = new QQmlEngine;
//	GuiEngine::instance(engine);

//	// Custom types registration
//	// - singletons
//	qmlRegisterSingletonType<GuiSplashWindow>(
//		QML_REGISTRAR, GUI_SPLASH_WINDOW_QML_NAME,
//		&GuiSplashWindow::qmlInstance
//	);
//	qmlRegisterSingletonType<GuiMainWindow>(
//		QML_REGISTRAR, GUI_MAIN_WINDOW_QML_NAME,
//		&GuiMainWindow::qmlInstance
//	);
//	// - instantiable
//	qmlRegisterType<GuiElementDetails>(
//		QML_REGISTRAR, GUI_ELEMENT_DETAILS_QML_NAME
//	);
//	// - enums
//	qmlRegisterUncreatableType<GuiQueryMatchType>(
//		QML_REGISTRAR, GUI_QUERY_MATCH_TYPE_QML_NAME,
//		"Not instantiable (enum)"
//	);
//	qmlRegisterUncreatableType<GuiDblpXmlLineType>(
//		QML_REGISTRAR, GUI_DBLP_XML_LINE_QML_NAME,
//		"Not instantiable (enum)"
//	);

//	IndexHandler *indexHandler;
//	IRModelIef *irmodel;
//	QueryResolver *queryResolver;

//	GuiSplashWindow &guiSplashWindow = GuiSplashWindow::instance();
//	GuiMainWindow &guiMainWindow = GuiMainWindow::instance();

//	if (!guiSplashWindow.create())
//		QUIT("Error occurred while creating SplashWindow");

//	if (!guiMainWindow.create())
//		QUIT("Error occurred while creating SplashWindow");

//	guiMainWindow.setShown(false);
//	guiSplashWindow.setShown(true);

//	// Load async
//	QFutureWatcher<void> indexLoadingWatcher;


//	QObject::connect(&indexLoadingWatcher,
//					 &QFutureWatcher<void>::finished,
//					 [&guiSplashWindow, &guiMainWindow]() {
//		// Load of everything finished, show main window
//		_dd(">>> Going to HIDE splash and show main window in thread: " << QThread::currentThreadId());
//		guiSplashWindow.setShown(false);
//		guiMainWindow.setShown(true);
//	});

////	QObject::connect(&indexLoadingWatcher,
////					 &QFutureWatcher<void>::progressTextChanged,
////					 [&guiSplashWindow](const QString &progress) {
////		_dd(">>> Setting status in thread: " << QThread::currentThreadId());
////		guiSplashWindow.setStatus(progress);
////	});

////	QObject::connect(&indexLoadingWatcher,
////					 &QFutureWatcher<void>::progressValueChanged,
////					 [&guiSplashWindow](int progress) {
////		_dd(">>> Setting status in thread: " << QThread::currentThreadId());
////		guiSplashWindow.setProgress(DOUBLE(progress) / 100);
////	});

//	QFuture<void> indexLoadingFuture =
//			QtConcurrent::run([&guiSplashWindow, &guiMainWindow,
//								&indexHandler, &irmodel, &queryResolver]() {

//		bool loadPositions = !arguments.dblpXmlFilePath.isEmpty();

//		auto splashProgressor = [&guiSplashWindow](double progress) {
//			guiSplashWindow.setProgress(progress);
//		};

//		// INDEX HANDLER

//		indexHandler = new IndexHandler(arguments.indexFolderPath, arguments.baseIndexName,
//										loadPositions);

//		// Connect to signals for detect progress

//		// Keys
//		QObject::connect(indexHandler, &IndexHandler::identifiersLoadStarted, [&guiSplashWindow]() {
//			guiSplashWindow.setStatus("Loading index file: keys");
//		});
//		QObject::connect(indexHandler, &IndexHandler::identifiersLoadProgress, splashProgressor);

//		// Vocabulary
//		QObject::connect(indexHandler, &IndexHandler::vocabularyLoadStarted, [&guiSplashWindow]() {
//			guiSplashWindow.setStatus("Loading index file: vocabulary");
//		});
//		QObject::connect(indexHandler, &IndexHandler::vocabularyLoadProgress, splashProgressor);

//		// Crossrefs
//		QObject::connect(indexHandler, &IndexHandler::crossrefsLoadStarted, [&guiSplashWindow]() {
//			guiSplashWindow.setStatus("Loading index file: crossrefs");
//		});
//		QObject::connect(indexHandler, &IndexHandler::crossrefsLoadProgress,
//						 splashProgressor);

//		if (loadPositions) {
//			// Positions
//			QObject::connect(indexHandler, &IndexHandler::positionsLoadStarted, [&guiSplashWindow]() {
//				guiSplashWindow.setStatus("Loading index file: positions");
//			});
//			QObject::connect(indexHandler, &IndexHandler::positionsLoadProgress,
//							 splashProgressor);
//		}

//		indexHandler->load();

//		// IR MODEL

//		irmodel = new IRModelIef(*indexHandler);

//		QObject::connect(irmodel, &IRModelIef::initStarted, [&guiSplashWindow]() {
//			guiSplashWindow.setStatus("Initializing IR Model");
//		});
//		QObject::connect(irmodel, &IRModelIef::initProgress, splashProgressor);

//		irmodel->init();

//		// QUERY RESOLVER (gratis)
//		queryResolver = new QueryResolver(irmodel);

//		// Set the resolver
//		guiMainWindow.setResolver(queryResolver);
//	});

//	indexLoadingWatcher.setFuture(indexLoadingFuture);


//	_dd(">>> haning on thread: " << QThread::currentThreadId());

//	return guiApp.exec();
//}

int main(int argc, char *argv[])
{
//	map_reduce_tests();
//	omp_tests();
//	insert_sort_tests();
//	thread_tests();
	PROF_FUNC_BEGIN

	if (argc < 2) {
		QUIT(HELP);
	}
	try {
		// Parse the arguments
		parseArguments(argc, argv);
		printArguments();

		if (arguments.mode == DblpSearcherMode::Index) {
			qInfo() << "Starting in INDEX mode";
			startIndexMode();
		}
		else if (arguments.mode == DblpSearcherMode::Search) {
			qInfo() << "Starting in SEARCH mode";
			startSearchMode();
		}
		else {
			qWarning() << "Arguments parsing faile; please provide a valid mode.";
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
