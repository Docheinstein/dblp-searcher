#include "query.h"

Logger Query::L = Logger::forClass("Query");

Query::~Query()
{
	// vv("Destructing query, deallocating query parts");
	qDeleteAll(mQueryParts);
}

void Query::addPart(QueryBasePart * queryPart)
{
	mQueryParts.append(queryPart);
}

Query::operator QString()
{
	QString s;
	int i = 0;
	for (auto it = mQueryParts.begin(); it != mQueryParts.end(); it++, i++) {
		s += QString::number(i) + ") " + **it + "\n";
	}

	return s;
}
