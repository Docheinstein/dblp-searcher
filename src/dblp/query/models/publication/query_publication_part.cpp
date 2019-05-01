#include "query_publication_part.h"
#include "commons/const/const.h"

QueryPublicationPart::QueryPublicationPart(
		QueryPublicationElementCase element, QueryPublicationFieldCase field) :
	mElement(element), mField(field)
{}


QueryPublicationElementType *QueryPublicationPart::element()
{
	return mElement;
}

QueryPublicationFieldType *QueryPublicationPart::field()
{
	return mField;
}

QueryPublicationPart::operator QString()
{
	QString s = mElement->string();
	if (mField != nullptr) {
		s += "@";
		s += mField->string();
	}

	return s + " | " + QueryBasePart::operator QString();
}
