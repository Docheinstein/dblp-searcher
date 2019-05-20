#ifndef CONST_H
#define CONST_H

#include <QString>

namespace Const {

	namespace Dblp {

		namespace Query {
			const QString TOKENS_SPLITTER = " ";
			const QString PHRASAL_MARK = "\"";
			const QString ELEMENT_FIELD_DIVISOR = ".";
			const QString QUERY_PART_SEARCH_END_MARK = ":";

			namespace Elements {
				const QString PUBLICATION = "publication";
				const QString ARTICLE = "article";
				const QString INCOLLECTION = "incollection";
				const QString INPROCEEDINGS = "inproc";
				const QString PHDTHESIS = "phThesis";
				const QString MASTERTHESIS = "masterThesis";

				const QString VENUE = "venue";
			}

			namespace Fields {
				const QString AUTHOR = "author";
				const QString TITLE = "title";
				const QString YEAR = "year";
				const QString PUBLISHER = "publisher";
			}
		}

		namespace Xml {
			const QString DBLP = "dblp"; // root element

			namespace Elements {
				const QString ARTICLE = "article";
				const QString INCOLLECTION = "incollection";
				const QString BOOK = "book";
				const QString INPROCEEDINGS = "inproceedings";
				const QString PROCEEDINGS = "proceedings";
				const QString PHDTHESIS = "phdthesis";
				const QString MASTERTHESIS = "masterthesis";
			}

			namespace Fields {
				const QString AUTHOR = "author";
				const QString TITLE = "title";
				const QString YEAR = "year";
				const QString JOURNAL = "journal";
				const QString PUBLISHER = "publisher";
				const QString CROSSREF = "crossref";
			}

			namespace Attributes {
				const QString KEY = "key";
			}
		}
	}
}

#endif // CONST_H
