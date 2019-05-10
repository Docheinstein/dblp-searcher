#ifndef QUERY_RESOLVER_H
#define QUERY_RESOLVER_H

#include <dblp/query/query/query.h>
#include <dblp/irmodel/base/ir_model.h>
#include <commons/log/loggable/loggable.h>
#include "dblp/shared/defs/defs.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/match/query_match.h"


class QueryResolver : protected Loggable {
public:
	QueryResolver(IRModel *irmodel);

	QList<QueryMatch> resolveQuery(const Query &query);

protected:
	LOGGING_OVERRIDE

	IRModel *mIrModel;
};

#endif // QUERY_RESOLVER_H
