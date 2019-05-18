#ifndef ELEMENT_FIELD_TYPE_H
#define ELEMENT_FIELD_TYPE_H

#include <QFlags>
#include <QString>

// article
// - author
// - title
// - year

// journal

// incollection
// - author
// - title
// - year
// - booktitle

// inproceedings
// - author
// - title
// - year
// - booktitle

// phdthesis
// - author
// - title
// - year

// masterthesis
// - author
// - title
// - year

// (journal)

// book
// - author
// - title
// - year
// - publisher

// proceedings
// - title
// - year
// - publisher
// - booktitle

enum class ElementFieldType {
	_Start = 1,

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
//	IncollectionBooktitle = IncollectionYear << 1,

	Incollection =	IncollectionAuthor | IncollectionTitle |
					IncollectionYear, // | IncollectionBooktitle,

	// Inproceedings

	InproceedingsAuthor = IncollectionYear << 1,
	InproceedingsTitle = InproceedingsAuthor << 1,
	InproceedingsYear = InproceedingsTitle << 1,
//	InproceedingsBooktitle = InproceedingsYear << 1,

	Inproceedings = InproceedingsAuthor | InproceedingsTitle |
					InproceedingsYear, //| InproceedingsBooktitle,

	// Phdthesis

	PhdthesisAuthor = InproceedingsYear << 1,
	PhdthesisTitle = PhdthesisAuthor << 1,
	PhdthesisYear = PhdthesisTitle << 1,

	Phdthesis = PhdthesisAuthor | PhdthesisTitle | PhdthesisYear,

	// Masterthesis

	MasterthesisAuthor = PhdthesisYear << 1,
	MasterthesisTitle = MasterthesisAuthor << 1,
	MasterthesisYear = MasterthesisTitle << 1,

	Masterthesis = MasterthesisAuthor | MasterthesisTitle | MasterthesisYear,

	// Book

	BookAuthor = MasterthesisYear << 1,
	BookTitle = BookAuthor << 1,
	BookYear = BookTitle << 1,
	BookPublisher = BookYear << 1,

	Book = BookAuthor | BookTitle | BookYear | BookPublisher,

	// Proceedings

	ProceedingsTitle = BookPublisher << 1,
	ProceedingsYear = ProceedingsTitle << 1,
	ProceedingsPublisher = ProceedingsYear << 1,
//	ProceedingsBooktitle = ProceedingsPublisher << 1,

	Proceedings = ProceedingsTitle | ProceedingsYear |
				  ProceedingsPublisher, // | ProceedingsBooktitle,

	_End = ProceedingsPublisher,

	// Extra: element

	Publication = Article | Incollection | Inproceedings |
				  Phdthesis | Masterthesis,

	Venue = Book | Proceedings | Journal,

	// Extra: field

	Author = ArticleAuthor | IncollectionAuthor | InproceedingsAuthor |
			PhdthesisAuthor | PhdthesisAuthor | MasterthesisAuthor | BookAuthor,

	Title = ArticleTitle | IncollectionTitle | InproceedingsTitle |
			PhdthesisTitle | PhdthesisTitle | MasterthesisTitle | BookTitle |
			ProceedingsTitle,

	Year = ArticleYear | IncollectionYear | InproceedingsYear |
			PhdthesisYear | PhdthesisYear | MasterthesisYear | BookYear |
			ProceedingsYear,

//	Booktitle = IncollectionBooktitle | ProceedingsBooktitle,

	Publisher = BookPublisher | ProceedingsPublisher,

	All = Publication | Venue,
};

QString elementFieldTypeString(ElementFieldType t);

Q_DECLARE_FLAGS(ElementFieldTypes, ElementFieldType);
Q_DECLARE_OPERATORS_FOR_FLAGS(ElementFieldTypes);

#endif // ELEMENT_FIELD_TYPE_H
