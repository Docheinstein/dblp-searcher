#ifndef INDEX_ELEMENT_FIELD_TYPE_H
#define INDEX_ELEMENT_FIELD_TYPE_H

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
	ArticleAuthor,
	ArticleTitle,
	ArticleYear,

	IncollectionAuthor,
	IncollectionTitle,
	IncollectionYear,
	IncollectionBooktitle,

	InproceedingsAuthor,
	InproceedingsTitle,
	InproceedingsYear,
	InproceedingsBooktitle,

	PhdthesisAuthor,
	PhdthesisTitle,
	PhdthesisYear,

	MasterthesisAuthor,
	MasterthesisTitle,
	MasterthesisYear,

	BookAuthor,
	BookTitle,
	BookYear,

	ProceedingsTitle,
	ProceedingsYear,
	ProceedingsPublisher,
	ProceedingsBooktitle
};

#endif // INDEX_ELEMENT_FIELD_TYPE_H
