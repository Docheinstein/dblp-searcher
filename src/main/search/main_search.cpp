#include "main_search.h"

#include <QGuiApplication>
#include <QIcon>
#include <QQmlEngine>
#include <QThread>

#include "main/args/args.h"
#include "commons/util/util.h"
#include "commons/config/app_config.h"
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

STATIC_LOGGING(Main, true)

int startSearchMode() {
	Q_ASSERT(arguments.mode == DblpSearcherMode::Search);

	// Gui application (must be before everything else)
	QGuiApplication guiApp(arguments.argc, arguments.argv);

	guiApp.setQuitOnLastWindowClosed(true);
	guiApp.setApplicationDisplayName(APPLICATION_NAME);
	guiApp.setWindowIcon(QIcon(":/img/dblp-icon.png"));

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

	// guiMainWindow.setShown(false);
	guiSplashWindow.setShown(true);

	_dd("Created windows on thread: " << QThread::currentThreadId());

	IndexLoadingController *controller = new IndexLoadingController(guiMainWindow, guiSplashWindow);
	controller->doIndexLoading();

	int execCode = guiApp.exec();

	_dd("Finished execution; returning with code: " << execCode);

	delete controller;

	_dd("Deleted indexing controller");

	return execCode;
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

IndexLoadingWorkerHandler::IndexLoadingWorkerHandler(
		GuiMainWindow &main, GuiSplashWindow &splash)
	:  mMain(main), mSplash(splash) {}

void IndexLoadingWorkerHandler::onStatusChanged(QString status)
{
//	_dd("Setting splash status '" << status << "' on thread: " << QThread::currentThreadId());
	mSplash.setStatus(status);
}

void IndexLoadingWorkerHandler::onProgressChanged(double progress)
{
//	_dd("Setting splash progress (" << progress << ") on thread: " << QThread::currentThreadId());
	mSplash.setProgress(progress);
}

void IndexLoadingWorkerHandler::onLoadingFinished(QueryResolver * resolver)
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

IndexLoadingController::IndexLoadingController(GuiMainWindow &main,
											   GuiSplashWindow &splash)
{

	mWorkerThread = new QThread();
	mWorker = new IndexLoadingWorker();
	mWorkerHandler = new IndexLoadingWorkerHandler(main, splash);

	mWorker->moveToThread(mWorkerThread);


	QObject::connect(mWorkerThread, &QThread::started,
					 mWorker, &IndexLoadingWorker::loadIndex,
					 Qt::QueuedConnection);

	QObject::connect(mWorker, &IndexLoadingWorker::statusChanged,
					 mWorkerHandler, &IndexLoadingWorkerHandler::onStatusChanged,
					 Qt::QueuedConnection);

	QObject::connect(mWorker, &IndexLoadingWorker::progressChanged,
					 mWorkerHandler, &IndexLoadingWorkerHandler::onProgressChanged,
					 Qt::QueuedConnection);

	QObject::connect(mWorker, &IndexLoadingWorker::loadFinished,
					 mWorkerHandler, &IndexLoadingWorkerHandler::onLoadingFinished,
					 Qt::QueuedConnection);

	QObject::connect(mWorker, &IndexLoadingWorker::loadFinished,
					 mWorker, &QObject::deleteLater);

	// Maybe works, but I prefer to postpone the deletion on the destructor
//	QObject::connect(mWorker, &IndexLoadingWorker::loadFinished,
//					 mWorkerHandler, &QObject::deleteLater);

	QObject::connect(mWorkerThread, &QThread::finished,
					 mWorkerThread, &QObject::deleteLater);

	QObject::connect(mWorker, &IndexLoadingWorker::loadFinished,
					 mWorkerThread, &QThread::quit);
}

IndexLoadingController::~IndexLoadingController()
{
//	_dd("Deleting worker");
//	delete mWorker;

	_dd("Deleting worker handler");
	delete mWorkerHandler;

//	_dd("Deleting thread");
//	delete mWorkerThread;
}

void IndexLoadingController::doIndexLoading()
{
	_dd("Hanging on thread: " << QThread::currentThreadId());
	mWorkerThread->start();
}
