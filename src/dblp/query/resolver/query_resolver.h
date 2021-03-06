#ifndef QUERY_RESOLVER_H
#define QUERY_RESOLVER_H

#include <QVector>
#include <QHash>

#include "commons/log/loggable/loggable.h"
#include "dblp/query/resolver/models/query_outcome.h"

class Query;
class IRModel;

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

// Needed for use QueryResolver with the Qt signals/slots paradigm
Q_DECLARE_METATYPE(QueryResolver);

#endif // QUERY_RESOLVER_H
