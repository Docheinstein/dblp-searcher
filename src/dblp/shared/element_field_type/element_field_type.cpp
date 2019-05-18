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
//	case ElementFieldType::IncollectionBooktitle:
//		return "IncollectionBooktitle";
	case ElementFieldType::Incollection:
		return "Incollection";

	case ElementFieldType::InproceedingsAuthor:
		return "InproceedingsAuthor";
	case ElementFieldType::InproceedingsTitle:
		return "InproceedingsTitle";
	case ElementFieldType::InproceedingsYear:
		return "InproceedingsYear";
//	case ElementFieldType::InproceedingsBooktitle:
//		return "InproceedingsBooktitle";
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

	case ElementFieldType::MasterthesisAuthor:
		return "MasterthesisAuthor";
	case ElementFieldType::MasterthesisTitle:
		return "MasterthesisTitle";
	case ElementFieldType::MasterthesisYear:
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
//	case ElementFieldType::ProceedingsBooktitle:
//		return "ProceedingsBooktitle";
	case ElementFieldType::Proceedings:
		return "Proceedings";

	case ElementFieldType::Publication:
		return "Publication";
	case ElementFieldType::Venue:
		return "Venue";
	case ElementFieldType::All:
		return "All";
	}

	return "<unknown>";
}
