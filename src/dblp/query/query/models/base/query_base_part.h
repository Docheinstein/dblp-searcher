#ifndef QUERY_BASE_PART_H
#define QUERY_BASE_PART_H

#include <QString>
#include <QStringList>
#include "dblp/shared/element_field_type/element_field_type.h"

class QueryElementType;
class QueryFieldType;

// Generic query part that has some tokens and eventually refers
// to a combination of element+field

class QueryBasePart {
public:
	virtual ~QueryBasePart() {}
	void addToken(const QString &token);

	QStringList tokens();

	// Combination of field+element;
	// e.g. All will have all the flags set
	virtual ElementFieldTypes elementFieldTypes() = 0;

	virtual QueryElementType *element() = 0;
	virtual QueryFieldType *field() = 0;

	virtual operator QString() const;

protected:
	QStringList mTokens;
};

#endif // QUERY_BASE_PART_H
