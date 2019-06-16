#include "query_venue_part.h"

QueryVenuePart::QueryVenuePart(
		QueryVenueElementCase element, QueryVenueFieldCase field) :
	mElement(element), mField(field)
{
	computeTypes();
}

void QueryVenuePart::computeTypes()
{
	if (mElement == QueryVenueElementType::VENUE) {
		if (!mField)
			mTypes = ElementFieldType::Venue;

		else if (mField == QueryVenueFieldType::TITLE)
			mTypes =
					ElementFieldType::BookTitle |
					ElementFieldType::ProceedingsTitle |

					// Technically this should be excluded, but we alllow
					// venue.title for a query with an article filter
					// and treat it as the journal name
					ElementFieldType::Journal;

		else if (mField == QueryVenueFieldType::PUBLISHER)
			mTypes =
					ElementFieldType::BookPublisher |
					ElementFieldType::ProceedingsPublisher;

					// We haven't any publishers info for the journals

		else
			// We should never come here
			mTypes = ElementFieldType::Venue;
	}

	else {
		// We should never come here
		mTypes = ElementFieldType::Venue;
	}
}

ElementFieldTypes QueryVenuePart::elementFieldTypes() { return mTypes; }
QueryElementType *QueryVenuePart::element() { return mElement; }
QueryFieldType *QueryVenuePart::field() { return mField; }

QueryVenuePart::operator QString() const
{
	QString s = mElement->string();
	if (mField != nullptr) {
		s += "@";
		s += mField->string();
	}

	return s + " | " + QueryBasePart::operator QString();
}
