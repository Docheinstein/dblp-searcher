#include "query_publication_part.h"
#include "commons/const/const.h"

QueryPublicationPart::QueryPublicationPart(
		QueryPublicationElementCase element, QueryPublicationFieldCase field) :
	mElement(element), mField(field)
{
	computeTypes();
}

ElementFieldTypes QueryPublicationPart::elementFieldTypes() { return mTypes; }
QueryElementType *QueryPublicationPart::element() { return mElement; }
QueryFieldType *QueryPublicationPart::field() { return mField; }

void QueryPublicationPart::computeTypes()
{

	// Compute the types based on the element and the fields

	if (mElement == QueryPublicationElementType::PUBLICATION) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Publication;

		else if (mField == QueryPublicationFieldType::AUTHOR)
			mTypes =
				ElementFieldType::ArticleAuthor |
				ElementFieldType::IncollectionAuthor |
				ElementFieldType::InproceedingsAuthor |
				ElementFieldType::PhdthesisAuthor |
				ElementFieldType::MastersthesisAuthor;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes =
				ElementFieldType::ArticleTitle |
				ElementFieldType::IncollectionTitle |
				ElementFieldType::InproceedingsTitle |
				ElementFieldType::PhdthesisTitle |
				ElementFieldType::MastersthesisTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes =
				ElementFieldType::ArticleYear |
				ElementFieldType::IncollectionYear |
				ElementFieldType::InproceedingsYear |
				ElementFieldType::PhdthesisYear |
				ElementFieldType::MastersthesisYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Publication;
	}

	else if (mElement == QueryPublicationElementType::ARTICLE) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Article;

		else if (mField == QueryPublicationFieldType::AUTHOR)
			mTypes = ElementFieldType::ArticleAuthor;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes = ElementFieldType::ArticleTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes = ElementFieldType::ArticleYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Article;
	}

	else if (mElement == QueryPublicationElementType::INCOLLECTION) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Incollection;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes = ElementFieldType::IncollectionTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes = ElementFieldType::IncollectionYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Incollection;
	}

	else if (mElement == QueryPublicationElementType::INPROCEEDINGS) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Inproceedings;

		else if (mField == QueryPublicationFieldType::AUTHOR)
			mTypes = ElementFieldType::InproceedingsAuthor;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes = ElementFieldType::InproceedingsTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes = ElementFieldType::InproceedingsYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Inproceedings;
	}

	else if (mElement == QueryPublicationElementType::PHDTHESIS) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Phdthesis;

		else if (mField == QueryPublicationFieldType::AUTHOR)
			mTypes = ElementFieldType::PhdthesisAuthor;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes = ElementFieldType::PhdthesisTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes = ElementFieldType::PhdthesisYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Phdthesis;
	}

	else if (mElement == QueryPublicationElementType::MASTERTHESIS) {
		if (!mField)
			// All the fields
			mTypes = ElementFieldType::Masterthesis;

		else if (mField == QueryPublicationFieldType::AUTHOR)
			mTypes = ElementFieldType::MastersthesisAuthor;

		else if (mField == QueryPublicationFieldType::TITLE)
			mTypes = ElementFieldType::MastersthesisTitle;

		else if (mField == QueryPublicationFieldType::YEAR)
			mTypes = ElementFieldType::MastersthesisYear;
		else
			// We should never come here
			mTypes = ElementFieldType::Masterthesis;
	}

	else {
		// We should never come here
		mTypes = ElementFieldType::Publication;
	}
}

QueryPublicationPart::operator QString() const
{
	QString s = mElement->string();
	if (mField != nullptr) {
		s += "@";
		s += mField->string();
	}

	return s + " | " + QueryBasePart::operator QString();
}
