#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QStringList>
#include <QFile>
#include <QUrl>

namespace Util {
	namespace Dblp {
		namespace Index {
			QString indexFilePath(const QString &indexPath,
								  const QString &indexBaseName,
								const QString &extension);
		}
	}

	namespace File {
		QString path(QStringList components);
		QString humanSize(const QFile &file);
		QString humanSize(qint64 filesize);
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

	namespace Resources {
		QUrl qmlUrl(const QString &name);
		QUrl imageUrl(const QString &fullname);
	}
}

#endif // UTIL_H
