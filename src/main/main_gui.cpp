#include "main_gui.h"

#include <QGuiApplication>
#include <QQmlEngine>
#include <QThread>
#include <QDebug>
#include <QFuture>
#include <QtConcurrentRun>
#include "gui/splash/splash_window.h"
#include "gui/main/main_window.h"
#include <dblp/index/handler/index_handler.h>
#include <dblp/irmodel/impl/ief/ir_model_ief.h>
#include <dblp/query/resolver/query_resolver.h>
#include "bootstrap/loader.h"

int mainGui(int argc, char *argv[],
			const char *indexPath, const char *baseIndexName)
{
	QGuiApplication guiApp(argc, argv);
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

	QtConcurrent::run([&indexPath, &baseIndexName,
					  &splashWindow, &mainWindow]() {

		auto splashProgressor = [&splashWindow](double progress) {
			splashWindow.setProgress(progress);
		};

		// INDEX HANDLER

		IndexHandler handler(indexPath, baseIndexName);

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
