#include <QObject>
#include <QThread>

class BackgroundThreadSender : public QThread {
	Q_OBJECT

public:
	void doJob();

signals:
	void firstJobFinished();
	void secondJobFinished();

	// QThread interface
protected:
	void run();
};

class BackgroundThreadReceiver : public QObject {
	Q_OBJECT

public slots:
	void onFirstJobFinished();
	void onSecondJobFinished();
};

void thread_tests();
