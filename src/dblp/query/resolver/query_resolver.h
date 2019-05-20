#ifndef QUERY_RESOLVER_H
#define QUERY_RESOLVER_H

#include <dblp/query/query/query.h>
#include <dblp/irmodel/base/ir_model.h>
#include <commons/log/loggable/loggable.h>
#include "dblp/shared/defs/defs.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/match/query_match.h"


struct QueryOutcome {
	QVector<QueryMatch> sortedQueryMatches;
	QHash<elem_serial, QVector<IndexMatch>> indexMatchesBySerial;
};

class QueryResolver : protected Loggable {
public:
	QueryResolver();
	QueryResolver(IRModel *irmodel);

	QueryOutcome resolveQuery(const Query &query);

	IRModel *irModel();

protected:
	LOGGING_OVERRIDE

	IRModel *mIrModel;
};
Q_DECLARE_METATYPE(QueryResolver);

#endif // QUERY_RESOLVER_H
