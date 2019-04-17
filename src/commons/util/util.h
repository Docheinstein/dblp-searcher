#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QStringList>
#include <QFile>

namespace Util {
	namespace Dblp {
		bool isElement(const QString &elementName);
//		bool isField(const QString &elementName);
	}

	namespace File {
		QString path(QStringList components);
		QString humanSize(const QFile &file);
	}

	namespace Html {
		QString entityToString(const QString &entity);
		QString entityNameToString(const QString &entityName);
	}

	namespace Indexing {
		QString sanitizeTerm(const QString &term);
	}
}

#endif // UTIL_H
