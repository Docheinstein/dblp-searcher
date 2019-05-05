#include "util.h"
#include "commons/const/const.h"
#include "commons/globals/globals.h"
#include "dblp/xml/models/types/xml_types.h"

#include <QTextDocument>
#include <QTime>

#define KB 1024
#define MB 1048576l

#define MS_IN_SEC 1000
#define MS_IN_MIN 60000


namespace Util {
	namespace Dblp {
		namespace Xml {
			bool isElement(const QString &elementName) {
				return	elementName == *XmlElementType::ARTICLE ||
						elementName == *XmlElementType::INCOLLECTION ||
						elementName == *XmlElementType::BOOK ||
						elementName == *XmlElementType::INPROCEEDINGS ||
						elementName == *XmlElementType::PROCEEDINGS ||
						elementName == *XmlElementType::PHDTHESIS ||
						elementName == *XmlElementType::MASTERTHESIS;
			}
		}

		namespace Query {
			QString queryPartSearch(const QString &element, const QString &field) {
				// Returns <publication>:
				// Or <publication>.<field>:
				if (field.isEmpty())
					return element + Const::Dblp::Query::QUERY_PART_SEARCH_END_MARK;

				return element + Const::Dblp::Query::ELEMENT_FIELD_DIVISOR +
						field + Const::Dblp::Query::QUERY_PART_SEARCH_END_MARK;
			}
		}
	}

	namespace File {
		QString path(QStringList components) {
			QString path = "";
			for (int i = 0; i < components.size(); ++i) {
				path += components.at(i);
				if (i < components.size() - 1 && !path.endsWith("/"))
					path += "/";
			}

			return path;
		}

		QString humanSize(const QFile &file) {
			qint64 sz = file.size();
			if (sz >= MB)
				return DEC(sz / MB) + "MB";
			if (sz >= KB)
				return DEC(sz / KB) + "KB";
			return DEC(sz) + "B";
		}
	}

	namespace Html {
		QString entityToString(const QString &entity) {
			QTextDocument doc;
			doc.setHtml(entity);
			return doc.toPlainText();
		}

		QString entityNameToString(const QString &entityName) {
			return entityToString("&" + entityName + ";");
		}
	}

	namespace String {
		QString sanitizeTerm(const QString &term) {
			// For now, apply the following filters
			// 1) To lower case
			// 2) Trim spaces (simplified is just like trim, but removes internal double spaces)
			// 3) Remove punctuation

			// Replace (at least the most common) punctuation with nothing
			return term
					.toLower()
					.simplified()
					.replace(QRegExp("[\\[\\]\\\\%&@#°*+\\-/|(),;.:_^]"), "");
		}
	}

	namespace Time {
		QString humanTime(int ms) {
			return QTime(0, 0, 0).addMSecs(ms).toString("m'm' s's'");
		}
	}

	namespace Qml {
		QUrl resourceUrl(const QString &name) {
			return QUrl(QString("qrc:/qml/") + name + ".qml");
		}
	}
}
