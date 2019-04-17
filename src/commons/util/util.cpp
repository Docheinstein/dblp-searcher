#include "util.h"
#include "commons/const/const.h"

#include <QTextDocument>

#define KB 1024
#define MB 1048576l

namespace Util {
	namespace Dblp {
		bool isElement(const QString &elementName) {
			return	elementName == Const::Dblp::Elements::ARTICLE ||
					elementName == Const::Dblp::Elements::INCOLLECTION ||
					elementName == Const::Dblp::Elements::BOOK ||
					elementName == Const::Dblp::Elements::INPROCEEDINGS ||
					elementName == Const::Dblp::Elements::PROCEEDINGS ||
					elementName == Const::Dblp::Elements::PHDTHESIS ||
					elementName == Const::Dblp::Elements::MASTERTHESIS;
		}
//		bool isField(const QString &elementName) {
//			return !isElement(elementName);
		//		}
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
				return QString::number(sz / MB) + "MB";
			if (sz >= KB)
				return QString::number(sz / KB) + "KB";
			return QString::number(sz) + "B";
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

	namespace Indexing {
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
}
