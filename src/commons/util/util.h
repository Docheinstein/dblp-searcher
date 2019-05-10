#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QUrl>

namespace Util {
	namespace Dblp {
		namespace Xml {
			bool isElement(const QString &elementName);
		}

		namespace Query {
			QString queryPartSearch(const QString &element, const QString &field = "");
		}

		namespace Index {
			QString indexFilePath(const QString &indexPath, const QString &indexBaseName,
								const QString &extension);
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

	namespace Qml {
		QUrl resourceUrl(const QString &name);
	}
}

#endif // UTIL_H
