#ifndef ELEMENT_FIELD_TYPE_H
#define ELEMENT_FIELD_TYPE_H

#include <QFlags>
#include <QString>

enum class ElementFieldType {
	_Start = 1, // Handly iterator begin

	// Article

	ArticleAuthor = _Start,
	ArticleTitle = ArticleAuthor << 1,
	ArticleYear = ArticleTitle << 1,

	Article = ArticleAuthor | ArticleTitle | ArticleYear,

	// Journal (name)

	Journal =  ArticleYear << 1,

	// Incollection

	IncollectionAuthor = Journal << 1,
	IncollectionTitle = IncollectionAuthor << 1,
	IncollectionYear = IncollectionTitle << 1,

	Incollection =	IncollectionAuthor | IncollectionTitle |
					IncollectionYear,

	// Inproceedings

	InproceedingsAuthor = IncollectionYear << 1,
	InproceedingsTitle = InproceedingsAuthor << 1,
	InproceedingsYear = InproceedingsTitle << 1,

	Inproceedings = InproceedingsAuthor | InproceedingsTitle |
					InproceedingsYear,

	// Phdthesis

	PhdthesisAuthor = InproceedingsYear << 1,
	PhdthesisTitle = PhdthesisAuthor << 1,
	PhdthesisYear = PhdthesisTitle << 1,

	Phdthesis = PhdthesisAuthor | PhdthesisTitle | PhdthesisYear,

	// Masterthesis

	MastersthesisAuthor = PhdthesisYear << 1,
	MastersthesisTitle = MastersthesisAuthor << 1,
	MastersthesisYear = MastersthesisTitle << 1,

	Masterthesis = MastersthesisAuthor | MastersthesisTitle | MastersthesisYear,

	// Book

	BookAuthor = MastersthesisYear << 1,
	BookTitle = BookAuthor << 1,
	BookYear = BookTitle << 1,
	BookPublisher = BookYear << 1,

	Book = BookAuthor | BookTitle | BookYear | BookPublisher,

	// Proceedings

	ProceedingsTitle = BookPublisher << 1,
	ProceedingsYear = ProceedingsTitle << 1,
	ProceedingsPublisher = ProceedingsYear << 1,

	Proceedings = ProceedingsTitle | ProceedingsYear |
				  ProceedingsPublisher,

	_End = ProceedingsPublisher, // Handly iterator end

	// Extra: element

	Publication = Article | Incollection | Inproceedings |
				  Phdthesis | Masterthesis,

	Venue = Book | Proceedings | Journal,

	// Extra: field

	Author = ArticleAuthor | IncollectionAuthor | InproceedingsAuthor |
			PhdthesisAuthor | PhdthesisAuthor | MastersthesisAuthor | BookAuthor,

	Title = ArticleTitle | IncollectionTitle | InproceedingsTitle |
			PhdthesisTitle | PhdthesisTitle | MastersthesisTitle | BookTitle |
			ProceedingsTitle,

	Year = ArticleYear | IncollectionYear | InproceedingsYear |
			PhdthesisYear | PhdthesisYear | MastersthesisYear | BookYear |
			ProceedingsYear,

	Publisher = BookPublisher | ProceedingsPublisher,

	All = Publication | Venue,

	// Extra
};

QString elementFieldTypeString(ElementFieldType t);

Q_DECLARE_FLAGS(ElementFieldTypes, ElementFieldType);
Q_DECLARE_OPERATORS_FOR_FLAGS(ElementFieldTypes);

#endif // ELEMENT_FIELD_TYPE_H
