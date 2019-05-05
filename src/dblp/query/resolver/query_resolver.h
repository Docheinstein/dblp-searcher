#ifndef QUERY_RESOLVER_H
#define QUERY_RESOLVER_H

#include <dblp/query/query/query.h>
#include <dblp/irmodel/base/ir_model.h>

typedef struct QueryMatch {
	quint32 elementId; // element id
} QueryMatch;

class QueryResolver {
public:
	QueryResolver(IRModel *irmodel);

	QSet<QueryMatch> resolveQuery(const Query &query);

private:
	static Logger L;

	IRModel *mIrModel;
};

#endif // QUERY_RESOLVER_H
