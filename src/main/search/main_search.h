#ifndef MAIN_SEARCH_H
#define MAIN_SEARCH_H

#include <QObject>
#include <QThread>

class GuiMainWindow;
class GuiSplashWindow;
class QueryResolver;

class IndexLoadingWorker : public QObject {
	Q_OBJECT

public slots:
	void loadIndex();

signals:
	void statusChanged(QString status);
	void progressChanged(double progress);
	void loadFinished(QueryResolver * resolver);
};


class IndexLoadingWorkerHandler : public QObject {
	Q_OBJECT

public:
	IndexLoadingWorkerHandler(GuiMainWindow &main,
							GuiSplashWindow &splash);

public slots:
	void onStatusChanged(QString status);
	void onProgressChanged(double progress);
	void onLoadingFinished(QueryResolver * resolver);

private:
	GuiMainWindow &mMain;
	GuiSplashWindow &mSplash;
};

class IndexLoadingController : public QObject {
public:
	IndexLoadingController(GuiMainWindow &main,
						   GuiSplashWindow &splash);
	~IndexLoadingController();

	void doIndexLoading();

private:
	QThread * mWorkerThread;
	IndexLoadingWorker * mWorker;
	IndexLoadingWorkerHandler * mWorkerHandler;
};

int startSearchMode();

#endif // MAIN_SEARCH_H
