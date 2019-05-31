#include "util.h"

#include <QTextDocument>
#include <QTime>

#include "commons/const/const.h"
#include "commons/globals/globals.h"

//#define KB 1024
//#define MB 1048576l
//#define GB 1073741824

#define KB 1000
#define MB 1000000
#define GB 1000000000

#define MS_IN_SEC 1000
#define MS_IN_MIN 60000

namespace Util {
	namespace Dblp {
		namespace Index {
			QString indexFilePath(const QString &indexPath,
							 const QString &indexBaseName,
							 const QString &extension) {
				return Util::File::path({indexPath, indexBaseName + extension});
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
			return humanSize(file.size());
		}

		QString humanSize(qint64 filesize) {
			if (filesize >= GB)
				return FLT(FLOAT(filesize) / GB) + "GB";
			if (filesize >= MB)
				return DEC(filesize / MB) + "MB";
			if (filesize >= KB)
				return DEC(filesize / KB) + "KB";
			return DEC(filesize) + "B";
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
			// 4) Convert to utf8 ?

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

	namespace Resources {
		QUrl qmlUrl(const QString &name) {
			return QUrl(QString("qrc:/qml/") + name + ".qml");
		}
		QUrl imageUrl(const QString &fullname) {
			return QUrl(QString("qrc:/img/") + fullname);
		}
	}
}
