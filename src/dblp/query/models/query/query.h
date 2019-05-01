#ifndef QUERY_H
#define QUERY_H

#include <QList>
#include <commons/logger/logger.h>
#include <dblp/query/models/base/query_base_part.h>

class Query {
public:
	static Logger L;
	~Query();
	void addPart(QueryBasePart * queryPart);

	operator QString();

protected:
	QList<QueryBasePart *> mQueryParts;
};

#endif // QUERY_H
