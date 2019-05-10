#ifndef QUERY_BASIC_PART_H
#define QUERY_BASIC_PART_H

#include <dblp/query/query/models/base/query_base_part.h>

class QueryBasicPart : public QueryBasePart {
public:
	ElementFieldTypes elementFieldTypes() override;
	QueryElementType *element() override;
	QueryFieldType *field() override;
};

#endif // QUERY_BASIC_PART_H
