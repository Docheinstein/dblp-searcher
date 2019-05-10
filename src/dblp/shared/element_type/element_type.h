#ifndef ELEMENT_TYPE_H
#define ELEMENT_TYPE_H

#include <QString>
#include "dblp/shared/element_field_type/element_field_type.h"

enum class ElementType {
	// Real
	Article = 1,
	Journal = Article << 1,
	Incollection = Journal << 1,
	Inproceedings = Incollection << 1,
	Phdthesis = Inproceedings << 1,
	Masterthesis = Phdthesis << 1,
	Book = Masterthesis << 1,
	Proceedings = Book << 1,

	// Extra
	Publication = Article | Incollection | Inproceedings |
				  Phdthesis | Masterthesis,
	Venue = Book | Proceedings | Journal,
};

QString elementTypeString(ElementType et);
ElementType elementTypeFromElementFieldType(ElementFieldType eft);

#endif // ELEMENT_TYPE_H
