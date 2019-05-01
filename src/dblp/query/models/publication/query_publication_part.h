#ifndef QUERY_PUBLICATION_PART_H
#define QUERY_PUBLICATION_PART_H

#include <dblp/query/models/base/query_base_part.h>
#include <dblp/query/models/types/query_types.h>

class QueryPublicationPart : public QueryBasePart {

public:
	QueryPublicationPart(QueryPublicationElementCase element,
						 QueryPublicationFieldCase field = nullptr);

	QueryPublicationElementType *element();
	QueryPublicationFieldType *field();

	operator QString() override;

protected:
	QueryPublicationElementType *mElement;
	QueryPublicationFieldType *mField;
};

#endif // QUERY_PUBLICATION_PART_H
