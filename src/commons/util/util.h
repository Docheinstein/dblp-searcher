#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QStringList>
#include <QFile>

namespace Util {
	namespace Dblp {
		namespace Xml {
			bool isElement(const QString &elementName);
		}

		namespace Query {
			QString queryPartSearch(const QString &element, const QString &field = "");
		}
	}

	namespace File {
		QString path(QStringList components);
		QString humanSize(const QFile &file);
	}

	namespace Time {
		QString humanTime(int ms);
	}

	namespace Html {
		QString entityToString(const QString &entity);
		QString entityNameToString(const QString &entityName);
	}


	namespace String {
		QString sanitizeTerm(const QString &term);
	}
}

#endif // UTIL_H
