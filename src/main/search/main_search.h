#ifndef MAIN_SEARCH_H
#define MAIN_SEARCH_H

#include <QObject>

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


class IndexLoadingThreadHandler : public QObject {
	Q_OBJECT

public:
	IndexLoadingThreadHandler(GuiMainWindow &main,
							GuiSplashWindow &splash);

public slots:
	void onStatusChanged(QString status);
	void onProgressChanged(double progress);
	void onLoadingFinished(QueryResolver * resolver);

private:
	GuiMainWindow &mMain;
	GuiSplashWindow &mSplash;
};

int startSearchMode();

#endif // MAIN_SEARCH_H
