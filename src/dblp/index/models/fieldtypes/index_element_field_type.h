#ifndef INDEX_ELEMENT_FIELD_TYPE_H
#define INDEX_ELEMENT_FIELD_TYPE_H

#include <QFlags>

// article
// - author
// - title
// - year
// (- journal)

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

enum IndexElementFieldType {
	IndexElementFieldTypeStart = 1,

	// Article

	ArticleAuthor = IndexElementFieldTypeStart,
	ArticleTitle = ArticleAuthor << 1,
	ArticleYear = ArticleTitle << 1,

	Article = ArticleAuthor | ArticleTitle | ArticleYear,

	// Incollection

	IncollectionAuthor = ArticleYear << 1,
	IncollectionTitle = IncollectionAuthor << 1,
	IncollectionYear = IncollectionTitle << 1,
	IncollectionBooktitle = IncollectionYear << 1,

	Incollection =	IncollectionAuthor | IncollectionTitle |
					IncollectionYear | IncollectionBooktitle,

	// Inproceedings

	InproceedingsAuthor = IncollectionBooktitle << 1,
	InproceedingsTitle = InproceedingsAuthor << 1,
	InproceedingsYear = InproceedingsTitle << 1,
	InproceedingsBooktitle = InproceedingsYear << 1,

	Inproceedings = InproceedingsAuthor | InproceedingsTitle |
					InproceedingsYear | InproceedingsBooktitle,

	// Phdthesis

	PhdthesisAuthor = InproceedingsBooktitle << 1,
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
	ProceedingsBooktitle = ProceedingsPublisher << 1,

	Proceedings = ProceedingsTitle | ProceedingsYear |
				  ProceedingsPublisher | ProceedingsBooktitle,

	// All
	All = Article | Incollection | Inproceedings | Phdthesis | Masterthesis |
		  Book | Proceedings,

	IndexElementFieldTypeEnd = ProceedingsBooktitle
};

Q_DECLARE_FLAGS(IndexElementFieldTypes, IndexElementFieldType);
Q_DECLARE_OPERATORS_FOR_FLAGS(IndexElementFieldTypes);

#endif // INDEX_ELEMENT_FIELD_TYPE_H
