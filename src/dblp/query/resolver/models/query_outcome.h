#ifndef QUERY_OUTCOME_H
#define QUERY_OUTCOME_H

#include <QVector>
#include <QHash>
#include "dblp/query/match/query_match.h"

struct QueryOutcome {
	QVector<QueryMatch> sortedQueryMatches;
	QHash<elem_serial, QVector<IndexMatch>> indexMatchesBySerial;
};

#endif // QUERY_OUTCOME_H
