#include "tests.h"

#include <QThread>
#include <QDebug>

void workerTests()
{
	QThread *t0 = new QThread();
	Worker *w = new Worker();
	WorkDeliverer *wd = new WorkDeliverer();

	w->moveToThread(t0);

	QObject::connect(wd, &WorkDeliverer::doWork,
					 w, &Worker::onDoWork,
					 Qt::QueuedConnection);

	QObject::connect(t0, &QThread::finished, []() {
		qInfo() << "Thread 0 finished its job";
	});

	t0->start();

	wd->deliverWorks();

}

void Worker::onDoWork(qint64 something)
{
	qInfo() << "onDoWork (" << something << ") on thread " << QThread::currentThread();
}

void WorkDeliverer::deliverWorks()
{
	qInfo() << "Delivering works from thread " << QThread::currentThread();
	for (int i = 0; i < 20; i++) {
		qInfo() << "Delivering (" << i << ")";
		emit doWork(i);
	}
}
