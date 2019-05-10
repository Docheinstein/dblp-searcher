#ifndef QUERY_PUBLICATION_PART_H
#define QUERY_PUBLICATION_PART_H

#include <dblp/query/query/models/base/query_base_part.h>
#include <dblp/query/query/models/types/query_types.h>

class QueryPublicationPart : public QueryBasePart {

public:
	QueryPublicationPart(QueryPublicationElementCase element,
						 QueryPublicationFieldCase field = nullptr);

	ElementFieldTypes elementFieldTypes() override;
	QueryElementType *element() override;
	QueryFieldType *field() override;

	operator QString() override;

protected:
	void computeTypes();

	ElementFieldTypes mTypes;
	QueryPublicationElementType *mElement;
	QueryPublicationFieldType *mField;
};

#endif // QUERY_PUBLICATION_PART_H
