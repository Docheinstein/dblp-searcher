#ifndef QUERY_BASE_PART_H
#define QUERY_BASE_PART_H

#include <QString>
#include <QStringList>
#include "dblp/shared/element_field_type/element_field_type.h"

class QueryElementType;
class QueryFieldType;

class QueryBasePart {
public:
	virtual ~QueryBasePart() {}
	void addToken(const QString &token);

	QStringList tokens();

	virtual ElementFieldTypes elementFieldTypes() = 0;
	virtual QueryElementType *element() = 0;
	virtual QueryFieldType *field() = 0;

	virtual operator QString();

protected:
	QStringList mTokens;
};

#endif // QUERY_BASE_PART_H
