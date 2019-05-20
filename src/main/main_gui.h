#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <QThread>

class GuiMainWindow;
class GuiSplashWindow;
class QueryResolver;

class IndexLoadingWorker : public QObject {
	Q_OBJECT

//public:
	// IndexLoadingWorker();

public slots:
	void loadIndex();

signals:
	void statusChanged(QString status);
	void progressChanged(double progress);
	void loadFinished(QueryResolver * resolver);
};


//class IndexLoadingThread : public QThread {
//	Q_OBJECT

//public:
//	// QThread interface

//protected:
//	void run();

//signals:
//	void statusChanged(QString status);
//	void progressChanged(double progress);
//	void loadingFinished(QueryResolver * resolver);

//private slots:
////	void onStatusChanged(QString status);
//	void onProgressChanged(double progress);
//};

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

#endif // MAIN_GUI_H
