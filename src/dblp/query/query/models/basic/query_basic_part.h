#ifndef QUERY_BASIC_PART_H
#define QUERY_BASIC_PART_H

#include <dblp/query/query/models/base/query_base_part.h>

// Query part not bound to any element+field (general query)

class QueryBasicPart : public QueryBasePart {
public:
	ElementFieldTypes elementFieldTypes() override;
	QueryElementType *element() override;
	QueryFieldType *field() override;
};

#endif // QUERY_BASIC_PART_H
