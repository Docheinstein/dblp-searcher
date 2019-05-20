#include "util.h"

#include <QTextDocument>
#include <QTime>

#include "commons/const/const.h"
#include "commons/globals/globals.h"

#define KB 1024
#define MB 1048576l

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

	namespace Qml {
		QUrl resourceUrl(const QString &name) {
			return QUrl(QString("qrc:/qml/") + name + ".qml");
		}
	}
}
