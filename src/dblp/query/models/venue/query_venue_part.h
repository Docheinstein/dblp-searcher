#ifndef QUERY_VENUE_PART_H
#define QUERY_VENUE_PART_H

#include <dblp/query/models/base/query_base_part.h>
#include <dblp/query/models/types/query_types.h>

class QueryVenuePart : public QueryBasePart {

public:
	QueryVenuePart(
			QueryVenueElementCase element,
			QueryVenueFieldCase field = nullptr);

	QueryVenueElementType *element(); // always VENUE
	QueryVenueFieldType *field();

	operator QString() override;

protected:
	QueryVenueElementType *mElement; // always VENUE
	QueryVenueFieldType *mField;
};


#endif // QUERY_VENUE_PART_H
