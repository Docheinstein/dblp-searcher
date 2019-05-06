#ifndef QUERY_RESOLVER_H
#define QUERY_RESOLVER_H

#include <dblp/query/query/query.h>
#include <dblp/irmodel/base/ir_model.h>
#include <commons/log/loggable/loggable.h>

typedef struct QueryMatch {
	quint32 elementId; // element id
} QueryMatch;

class QueryResolver : protected Loggable {
public:
	QueryResolver(IRModel *irmodel);

	QSet<QueryMatch> resolveQuery(const Query &query);

protected:
	LOGGING_OVERRIDE

	IRModel *mIrModel;
};

#endif // QUERY_RESOLVER_H
