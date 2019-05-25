#include <QObject>

class Worker : public QObject {
	Q_OBJECT

public slots:
	void onDoWork(qint64 something);
};

class WorkDeliverer : public QObject {
	Q_OBJECT

public:
	void deliverWorks();

signals:
	void doWork(int something);
};

void workerTests();
