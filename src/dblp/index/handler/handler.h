#ifndef INDEX_HANDLER_H
#define INDEX_HANDLER_H

#include <QDataStream>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>

#include <dblp/index/reference/index_term_ref.h>

#include <commons/logger/logger.h>

#include <dblp/index/key/index_key.h>

class IndexHandler
{
public:
	IndexHandler(QString &indexPath, QString &baseName);

	void load();

	void debug_findArticlesOfAuthor(const QString &author);

private:
	Logger L = Logger::forClass("IndexHandler");

	QString mIndexPath;
	QString mBaseIndexName;

	QFile mKeysFile;
	QTextStream mKeysStream;

	QFile mVocabularyFile;
	QDataStream mVocabularyStream;

	QFile mPostingListFile;
	QDataStream mPostingListStream;

	QList<IndexKey> mKeys;
	QHash<QString, IndexTermRef> mVocabulary;

	void loadKeys();
	void loadVocabulary();

	void debug_printKeys();
	void debug_printVocabulary();
};

#endif // INDEX_HANDLER_H
