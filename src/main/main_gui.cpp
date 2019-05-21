#include "main_gui.h"

#include <QGuiApplication>
#include <QQmlEngine>
#include <QThread>

#include "main/args.h"
#include "dblp/irmodel/impl/ief/ir_model_ief.h"
#include "dblp/query/resolver/query_resolver.h"
#include "gui/engine/gui_engine.h"
#include "gui/windows/splash/gui_splash_window.h"
#include "gui/windows/main/gui_main_window.h"
#include "gui/components/dblp_xml_lines/gui_dblp_xml_lines.h"
#include "gui/components/element_details/gui_element_details.h"

#define QML_URI				"DblpSearcher"
#define QML_VERSION_MAJOR	1
#define QML_REVISION		0

#define QML_REGISTRAR QML_URI, QML_VERSION_MAJOR, QML_REVISION

#if DEBUG
// Just for avoid warnings about unused vars
STATIC_LOGGING(Main, true)
#endif

int startSearchMode() {
	Q_ASSERT(arguments.mode == DblpSearcherMode::Search);

	// Gui application (must be before everything else)
	QGuiApplication guiApp(arguments.argc, arguments.argv);

	// Global qml engine
	QQmlEngine *engine = new QQmlEngine;
	GuiEngine::instance(engine);

	// Custom types registration
	// - singletons
	qmlRegisterSingletonType<GuiSplashWindow>(
		QML_REGISTRAR, GUI_SPLASH_WINDOW_QML_NAME,
		&GuiSplashWindow::qmlInstance
	);
	qmlRegisterSingletonType<GuiMainWindow>(
		QML_REGISTRAR, GUI_MAIN_WINDOW_QML_NAME,
		&GuiMainWindow::qmlInstance
	);
	// - instantiable
	qmlRegisterType<GuiElementDetails>(
		QML_REGISTRAR, GUI_ELEMENT_DETAILS_QML_NAME
	);
	// - enums
	qmlRegisterUncreatableType<GuiQueryMatchType>(
		QML_REGISTRAR, GUI_QUERY_MATCH_TYPE_QML_NAME,
		"Not instantiable (enum)"
	);
	qmlRegisterUncreatableType<GuiDblpXmlLineType>(
		QML_REGISTRAR, GUI_DBLP_XML_LINE_QML_NAME,
		"Not instantiable (enum)"
	);
	// Query resolver for background loading
	qRegisterMetaType<QueryResolver>();

	GuiSplashWindow &guiSplashWindow = GuiSplashWindow::instance();
	GuiMainWindow &guiMainWindow = GuiMainWindow::instance();

	if (!guiSplashWindow.create())
		QUIT("Error occurred while creating SplashWindow");

	if (!guiMainWindow.create())
		QUIT("Error occurred while creating SplashWindow");

	guiMainWindow.setShown(false);
	guiSplashWindow.setShown(true);

	_dd("Created windows on thread: " << QThread::currentThread());

	QThread * workerThread = new QThread();
	IndexLoadingWorker *worker = new IndexLoadingWorker();
	worker->moveToThread(workerThread);

	IndexLoadingThreadHandler loadingThreadHandler(guiMainWindow, guiSplashWindow);

	QObject::connect(workerThread, &QThread::started,
					 worker, &IndexLoadingWorker::loadIndex,
					 Qt::QueuedConnection);

	QObject::connect(worker, &IndexLoadingWorker::statusChanged,
					 &loadingThreadHandler, &IndexLoadingThreadHandler::onStatusChanged,
					 Qt::QueuedConnection);

	QObject::connect(worker, &IndexLoadingWorker::progressChanged,
					 &loadingThreadHandler, &IndexLoadingThreadHandler::onProgressChanged,
					 Qt::QueuedConnection);

	QObject::connect(worker, &IndexLoadingWorker::loadFinished,
					 &loadingThreadHandler, &IndexLoadingThreadHandler::onLoadingFinished,
					 Qt::QueuedConnection);

	workerThread->start();

	_dd("Hanging on : " << QThread::currentThreadId());

	return guiApp.exec();
}

void IndexLoadingWorker::loadIndex()
{
	bool loadPositions = !arguments.dblpXmlFilePath.isEmpty();

	IndexHandler *indexHandler;
	IRModelIef *irmodel;
	QueryResolver *queryResolver;

	// INDEX HANDLER

	indexHandler = new IndexHandler(arguments.indexFolderPath, arguments.baseIndexName,
									loadPositions);

	// Connect to signals for detect progress

	// Identifiers signals
	QObject::connect(indexHandler, &IndexHandler::identifiersLoadStarted,
					 [this]() { emit statusChanged("Loading index file: identifiers"); });
	QObject::connect(indexHandler, &IndexHandler::identifiersLoadProgress,
					 this, &IndexLoadingWorker::progressChanged);

	// Vocabulary signals
	QObject::connect(indexHandler, &IndexHandler::vocabularyLoadStarted,
					 [this]() { emit statusChanged("Loading index file: vocabulary"); });
	QObject::connect(indexHandler, &IndexHandler::vocabularyLoadProgress,
					 this, &IndexLoadingWorker::progressChanged);

	// Vocabulary signals
	QObject::connect(indexHandler, &IndexHandler::crossrefsLoadStarted,
					 [this]() { emit statusChanged("Loading index file: crossrefs"); });
	QObject::connect(indexHandler, &IndexHandler::crossrefsLoadProgress,
					 this, &IndexLoadingWorker::progressChanged);

	if (loadPositions) {
		// Positions signals
		QObject::connect(indexHandler, &IndexHandler::positionsLoadStarted,
						 [this]() { emit statusChanged("Loading index file: positions"); });
		QObject::connect(indexHandler, &IndexHandler::positionsLoadProgress,
						 this, &IndexLoadingWorker::progressChanged);
	}

	indexHandler->load();

	// IR MODEL

	irmodel = new IRModelIef(*indexHandler);

	// IR model signals
	QObject::connect(irmodel, &IRModelIef::initStarted,
					 [this]() { emit statusChanged("Initializing IR Model"); });
	QObject::connect(irmodel, &IRModelIef::initProgress,
					 this, &IndexLoadingWorker::progressChanged);

	irmodel->init();

	// QUERY RESOLVER (gratis)
	queryResolver = new QueryResolver(irmodel);

	emit loadFinished(queryResolver);
}

IndexLoadingThreadHandler::IndexLoadingThreadHandler(
		GuiMainWindow &main, GuiSplashWindow &splash)
	:  mMain(main), mSplash(splash) {}

void IndexLoadingThreadHandler::onStatusChanged(QString status)
{
//	_dd("Setting splash status '" << status << "' on thread: " << QThread::currentThreadId());
	mSplash.setStatus(status);
}

void IndexLoadingThreadHandler::onProgressChanged(double progress)
{
//	_dd("Setting splash progress (" << progress << ") on thread: " << QThread::currentThreadId());
	mSplash.setProgress(progress);
}

void IndexLoadingThreadHandler::onLoadingFinished(QueryResolver * resolver)
{
	_dd("Index loading finished");

	// Set the resolver
	mMain.setResolver(resolver);

	// Hide splash, show main
	mSplash.setShown(false);
	mMain.setShown(true);
	_dd("Going to hide splash and show main window on thread: "
		<< QThread::currentThreadId());
}
