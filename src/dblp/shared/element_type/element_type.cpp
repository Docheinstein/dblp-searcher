#include "element_type.h"

#include "commons/globals/globals.h"

QString elementTypeString(ElementType t)
{
	switch (t) {
	case ElementType::Article:
		return "Article";

	case ElementType::Journal:
		return "Journal";

	case ElementType::Incollection:
		return "Incollection";

	case ElementType::Inproceedings:
		return "Inproceedings";

	case ElementType::Phdthesis:
		return "Phdthesis";

	case ElementType::Masterthesis:
		return "Masterthesis";

	case ElementType::Book:
		return "Book";

	case ElementType::Proceedings:
		return "Proceedings";

	case ElementType::Publication:
		return "<unknown>"; // Not usable

	case ElementType::Venue:
		return "<unknown>"; // Not usable
	}

	return "<unknown>";
}

ElementType elementTypeFromElementFieldType(ElementFieldType eft)
{
	int ieft = INT(eft);

	if ((ieft & INT(ElementFieldType::Article)) == ieft)
		return ElementType::Article;

	if ((ieft & INT(ElementFieldType::Journal)) == ieft)
		return ElementType::Journal;

	if ((ieft & INT(ElementFieldType::Incollection)) == ieft)
		return ElementType::Incollection;

	if ((ieft & INT(ElementFieldType::Inproceedings)) == ieft)
		return ElementType::Inproceedings;

	if ((ieft & INT(ElementFieldType::Phdthesis)) == ieft)
		return ElementType::Phdthesis;

	if ((ieft & INT(ElementFieldType::Masterthesis)) == ieft)
		return ElementType::Masterthesis;

	if ((ieft & INT(ElementFieldType::Book)) == ieft)
		return ElementType::Book;

	if ((ieft & INT(ElementFieldType::Proceedings)) == ieft)
		return ElementType::Proceedings;

	// Publication and Venue are not usable

	throw "Illegal ElementFieldType provided to elementTypeFromElementFieldType()";
}
