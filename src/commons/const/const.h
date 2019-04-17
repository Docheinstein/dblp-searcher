#ifndef CONST_H
#define CONST_H

#include <QString>

namespace Const {
	namespace Dblp {
		namespace Elements {
			const QString ARTICLE = "article";
			const QString INCOLLECTION = "incollection";
			const QString BOOK = "book";
			const QString INPROCEEDINGS = "inproceedings";
			const QString PROCEEDINGS = "proceedings";
			const QString PHDTHESIS = "phdthesis";
			const QString MASTERTHESIS = "masterthesis";
		}

		namespace Attributes {
			const QString KEY = "key";
		}

		namespace Fields {
			const QString AUTHOR = "author";
			const QString TITLE = "title";
			const QString YEAR = "year";
			const QString JOURNAL = "journal";
			const QString PUBLISHER = "publisher";
			const QString BOOKTITLE = "booktitle";
			const QString CROSSREF = "crossref";
		}
	}
}

#endif // CONST_H
