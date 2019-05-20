#include <QString>
#include <QDebug>
#include "commons/profiler/profiler.h"
#include "dblp/index/models/match/index_match.h"
#include <QtConcurrent>
#include <QVector>
#include <omp.h>
#include <QMutex>
#include <QHash>
#include <QObject>
#include "tests.h"
#include <QGuiApplication>
#include "args.h"

//#define CURTHREAD QThread::currentThreadId()

#define N        1000000
int data[N];

//void backgroundThread() {
//	qInfo() << "Doing first job on background thread: " << QThread::currentThreadId();
//	QThread::sleep(1);
//	emit firstJobFinished();
//	qInfo() << "Doing second job on background thread: " << QThread::currentThreadId();
//	QThread::sleep(2);
//	emit secondJobFinished();
//}

void thread_tests() {
	QCoreApplication app(arguments.argc, arguments.argv);


//	QFutureWatcher<void> indexLoadingWatcher;

//	QObject::connect(&indexLoadingWatcher,
//					 &QFutureWatcher<void>::finished,
//					 [&guiSplashWindow, &guiMainWindow]() {
//		// Load of everything finished, show main window
//		guiSplashWindow.setShown(false);
//		guiMainWindow.setShown(true);
//		_dd("Going to hide splash and show main window on thread: "
//			<< QThread::currentThread());
//	});

//	QObject::connect(&indexLoadingWatcher)

//	QObject::connect(future, &QFuture::)

//	QObject::connect(&futureWatcher, SLOT(slotFunc()),
//					 SIGNAL(firstJobFinished()),
//					 Qt::QueuedConnection);

//	QFuture<void> future = QtConcurrent::run(backgroundThread);
//	QFutureWatcher<void> futureWatcher;
	BackgroundThreadSender backgroundThreadSend;
	BackgroundThreadReceiver *backgroundThreadRecv = new BackgroundThreadReceiver();
	qInfo() << "Main thread: " << QThread::currentThreadId();

//	bool ok = QObject::connect(&backgroundThreadSend, &BackgroundThreadSender::firstJobFinished,
//					 [&backgroundThreadRecv]() {
//		qInfo() << "Ehy from: " << QThread::currentThreadId();
//		qInfo() << "I want: " << backgroundThreadRecv.thread();
//		;
//	});

//	qInfo() << "Connection 1 ok: " << ok;

//	bool ok = QObject::connect(&backgroundThreadSend, &BackgroundThreadSender::secondJobFinished,
//					 backgroundThreadRecv, &BackgroundThreadReceiver::onSecondJobFinished,
//					 Qt::QueuedConnection);
	bool ok = QObject::connect(&backgroundThreadSend, &BackgroundThreadSender::secondJobFinished,
					 backgroundThreadRecv, &BackgroundThreadReceiver::onSecondJobFinished,
					 Qt::QueuedConnection);

	qInfo() << "Connection 2 ok: " << ok;


//	backgroundThreadSend.dumpObjectInfo();
//	backgroundThreadRecv.dumpObjectInfo();
//	QFuture<void> future = QtConcurrent::run(&backgroundThreadSend,
//											 &BackgroundThreadSender::doJob);
//	futureWatcher.setFuture(future);

	backgroundThreadSend.start();

	app.exec();
}


//void BackgroundThreadSender::doJob() {
//	qInfo() << "Doing first job on background thread: " << QThread::currentThreadId();
//	QThread::sleep(1);
//	emit BackgroundThreadSender::firstJobFinished();
//	qInfo() << "Doing second job on background thread: " << QThread::currentThreadId();
//	QThread::sleep(2);
//	emit BackgroundThreadSender::secondJobFinished();
//}


typedef struct Element {
	int serial;
	float score;
} Element;

bool operator <(const Element &e1, const Element &e2) {
	return e1.score < e2.score;
}

[[noreturn]] void insert_sort_tests() {
	srand(time(nullptr));

	QMap<float, Element> hash;
	QVector<Element> vect;

	PROF_BEGIN(hash)
		PROF_BEGIN1(insertHash)
		for (int i = 0; i < N; i++) {
			float sc = rand() % 10000 / FLOAT(100);
			hash.insert(sc, {i, sc});
		}
		const QList<Element> hashValues = hash.values();
		PROF_END(insertHash)
	PROF_END(hash)

	PROF_BEGIN(vector)

		PROF_BEGIN1(insertVector)
		for (int i = 0; i < N; i++) {
			float sc = rand() % 10000 / FLOAT(100);
			vect.append({i, sc});
		}
		PROF_END(insertVector)
		PROF_BEGIN1(sortVector)
		std::sort(vect.begin(), vect.end());
		PROF_END(sortVector)

	PROF_END(vector)

	prof_print();

	exit(-1);
}

[[noreturn]] void omp_tests() {
//	int i;
//	int result = 1000000;

//	#pragma omp declare reduction(minabs : int :              \
//	  omp_out = abs(omp_in) > omp_out ? omp_out : abs(omp_in))
////	  initializer (omp_priv=1000000))

//	#pragma omp parallel for reduction(minabs:result)
//	for (i=5; i<N; i++) {
//	  if (abs(data[i]) < result) {
//		result = abs(data[i]);
//	  }
//	}

//	qInfo() << "Result: " << result;
//	exit(-1);

	QVector<int> array({1, 2, 3, 5, 2, 53, 15, 125, 4124, 2});

	#pragma omp declare reduction(array_to_hash : QHash<int, int> : \
		omp_out.unite(omp_in))

	QHash<int, int> result;
	#pragma omp parallel for reduction(array_to_hash:result)
	for (int i = 0; i < array.size(); i++) {
		result.insert(i, i * 2);
	}

	for (auto it = result.begin(); it != result.end(); it++) {
		qInfo() << it.key() << " - " << it.value();
	}

	exit(-1);
}

void hash_test_uint() {
	const int insertions = 36036760;
	const int TOP = 30;

	typedef uint hits_number;

	QHash<uint, hits_number> hits;
	QMap<hits_number, uint> hitsReverse;

	for (int i = 0; i < insertions; i++) {
		uint h = qHash(i % 20);
		hits_number colls = hits.value(h, 0);
		colls++;
		hits.insert(h, colls);
	}

	for (auto it = hits.begin();
		 it != hits.end();
		 it++) {

		uint hsh = it.key();
		hits_number hits = it.value();

		qInfo() << "Hash: " << hsh << " | Hits: " << hits;

		// Insert reverse collision
		hitsReverse.insert(hits, hsh);
	}

	qInfo() << "---";

	quint64 totalCollisions = 0;

	int i = 0;
	for (auto it = hitsReverse.end() - 1;
		 i < TOP && i < hitsReverse.size();
		 --it, i++) {

		uint hsh = it.value();
		hits_number hits = it.key();

		qInfo() << "Hash: " << hsh << " | Hits: " << hits;

		// Sum collision number
		totalCollisions += (hits - 1);
	}

	qInfo() << "# Insertions: " << insertions;
	qInfo() << "# Collisions: " << totalCollisions;
	qInfo() << "Avg collision rateo: " << DOUBLE(totalCollisions) / insertions;
}

void hash_test_index_matches() {
	const int NUM = 8388608 / 10;
	const int TOP = 30;

	// Index Match

	typedef uint hits_number;

	quint64 insertions = 0;

	QHash<uint, hits_number> indexMatchHits;
	QMap<hits_number, uint> indexMatchHitsReverse;

	for (int i = 0; i < NUM; i++) {
		for  (int j = 0; j < rand() % 20; j++) {
			insertions++;
			// "Random" IndexMatch
			IndexMatch m;
			m.elementSerial = i * ((rand() % 10) + 1);
			m.fieldType = static_cast<ElementFieldType>(1 << (rand() % 22));
			m.matchPosition = rand() % 17;
			m.fieldNumber = rand() % 100 == 0 ? (rand() % 5) : 0;

			uint h = qHash(m);
			hits_number hits = indexMatchHits.value(h, 0);
			hits++;
			indexMatchHits.insert(h, hits);
		}

	}

	quint64 totalCollisions = 0;

	for (auto it = indexMatchHits.begin();
		 it != indexMatchHits.end();
		 it++) {
//		qInfo() << "Hash: " << it.key() << " | Collisions: " << it.value();

		hits_number hits = it.value();

		// Insert reverse collision
		indexMatchHitsReverse.insert(hits, it.key());
	}

	// Print collisions
	int i = 0;
	for (auto it = indexMatchHitsReverse.end() - 1;
		 i < TOP && i < indexMatchHitsReverse.size();
		 --it, i++) {
		hits_number hits = it.key();

		qInfo() << "Hash: " << it.value() << " | Hits: " << hits;

		// Sum collision number
		totalCollisions += (hits - 1);
	}

	qInfo() << "# Insertions: " << insertions;
	qInfo() << "# Collisions: " << totalCollisions;
	qInfo() << "Avg collision rateo: " << DOUBLE(totalCollisions) / insertions;
//	qInfo() << "Expected collisions: " << totalCollisions / UINT64(indexMatchCollisions.size());

}

[[noreturn]] void hash_tests() {
	srand(time(nullptr));

//	hash_test_uint();
	hash_test_index_matches();
	exit(-1);
}


// U function(const T &t);
int map_func(const int &val) {
	return val / 2;
}

// V function(T &result, const U &intermediate)
void reduce_func(double &reduceResult, const int &mapResult) {
	reduceResult += mapResult;
}

[[noreturn]] void map_reduce_tests() {
	QVector<int> arr({10, 5, 5, 2, 3, 1, 5, 6, 8, 3, 11});
	double res = QtConcurrent::blockingMappedReduced(arr, map_func, reduce_func);
	qInfo() << "Result: " << res;
	exit(-1);
}


void BackgroundThreadReceiver::onFirstJobFinished()
{
	qInfo() << "Notified about first job finished on thread: " << QThread::currentThreadId();
}

void BackgroundThreadReceiver::onSecondJobFinished()
{
	qInfo() << "Invoked...";
	qInfo() << "Notified about second job finished on thread: " << QThread::currentThreadId();
}

void BackgroundThreadSender::doJob()
{
	qInfo() << "Doing first job on background thread: " << QThread::currentThreadId();
	QThread::sleep(1);
	emit firstJobFinished();
	qInfo() << "Doing second job on background thread: " << QThread::currentThreadId();
	QThread::sleep(2);
	emit secondJobFinished();
}

void BackgroundThreadSender::run()
{
	doJob();
}
