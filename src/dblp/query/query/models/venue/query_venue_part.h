#ifndef QUERY_VENUE_PART_H
#define QUERY_VENUE_PART_H

#include <dblp/query/query/models/base/query_base_part.h>
#include <dblp/query/query/models/types/query_types.h>

class QueryVenuePart : public QueryBasePart {

public:
	QueryVenuePart(
			QueryVenueElementCase element,
			QueryVenueFieldCase field = nullptr);

	ElementFieldTypes elementFieldTypes() override;
	QueryElementType *element() override;
	QueryFieldType *field() override;

	operator QString() const override;

protected:
	void computeTypes();

	ElementFieldTypes mTypes;
	QueryVenueElementType *mElement; // always VENUE
	QueryVenueFieldType *mField;
};


#endif // QUERY_VENUE_PART_H
