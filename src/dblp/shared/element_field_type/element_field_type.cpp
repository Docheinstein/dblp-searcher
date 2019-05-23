#include "element_field_type.h"

QString elementFieldTypeString(ElementFieldType t) {
	switch (t) {
	case ElementFieldType::ArticleAuthor:
		return "ArticleAuthor";
	case ElementFieldType::ArticleTitle:
		return "ArticleTitle";
	case ElementFieldType::ArticleYear:
		return "ArticleYear";
	case ElementFieldType::Article:
		return "Article";

	case ElementFieldType::Journal:
		return "Journal";

	case ElementFieldType::IncollectionAuthor:
		return "IncollectionAuthor";
	case ElementFieldType::IncollectionTitle:
		return "IncollectionTitle";
	case ElementFieldType::IncollectionYear:
		return "IncollectionYear";
	case ElementFieldType::Incollection:
		return "Incollection";

	case ElementFieldType::InproceedingsAuthor:
		return "InproceedingsAuthor";
	case ElementFieldType::InproceedingsTitle:
		return "InproceedingsTitle";
	case ElementFieldType::InproceedingsYear:
		return "InproceedingsYear";
	case ElementFieldType::Inproceedings:
		return "Inproceedings";

	case ElementFieldType::PhdthesisAuthor:
		return "PhdthesisAuthor";
	case ElementFieldType::PhdthesisTitle:
		return "PhdthesisTitle";
	case ElementFieldType::PhdthesisYear:
		return "PhdthesisYear";
	case ElementFieldType::Phdthesis:
		return "Phdthesis";

	case ElementFieldType::MastersthesisAuthor:
		return "MasterthesisAuthor";
	case ElementFieldType::MastersthesisTitle:
		return "MasterthesisTitle";
	case ElementFieldType::MastersthesisYear:
		return "MasterthesisYear";
	case ElementFieldType::Masterthesis:
		return "Masterthesis";

	case ElementFieldType::BookAuthor:
		return "BookAuthor";
	case ElementFieldType::BookTitle:
		return "BookTitle";
	case ElementFieldType::BookYear:
		return "BookYear";
	case ElementFieldType::BookPublisher:
		return "BookPublisher";
	case ElementFieldType::Book:
		return "Book";

	case ElementFieldType::ProceedingsTitle:
		return "ProceedingsTitle";
	case ElementFieldType::ProceedingsYear:
		return "ProceedingsYear";
	case ElementFieldType::ProceedingsPublisher:
		return "ProceedingsPublisher";
	case ElementFieldType::Proceedings:
		return "Proceedings";

	case ElementFieldType::Publication:
		return "Publication";
	case ElementFieldType::Venue:
		return "Venue";
	case ElementFieldType::Author:
		return "Author";
	case ElementFieldType::Title:
		return "Title";
	case ElementFieldType::Year:
		return "Year";
	case ElementFieldType::Publisher:
		return "Publisher";
	case ElementFieldType::All:
		return "All";
	}

	return "<unknown>";

}
