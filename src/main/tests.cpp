#include <QString>
#include <QDebug>
#include "commons/profiler/profiler.h"
#include "dblp/index/models/match/index_match.h"


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
