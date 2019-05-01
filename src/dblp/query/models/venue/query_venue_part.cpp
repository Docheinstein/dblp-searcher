#include "query_venue_part.h"

QueryVenuePart::QueryVenuePart(
		QueryVenueElementCase element, QueryVenueFieldCase field) :
	mElement(element), mField(field)
{}

QueryVenueElementType * QueryVenuePart::element()
{
	return mElement;
}

QueryVenueFieldType * QueryVenuePart::field()
{
	return mField;
}

QueryVenuePart::operator QString()
{
	QString s = mElement->string();
	if (mField != nullptr) {
		s += "@";
		s += mField->string();
	}

	return s + " | " + QueryBasePart::operator QString();
}
