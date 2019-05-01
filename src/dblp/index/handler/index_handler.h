#ifndef INDEX_HANDLER_H
#define INDEX_HANDLER_H

#include <QDataStream>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>
#include "dblp/index/models/key/index_key.h"
#include "dblp/index/models/reference/index_term_ref.h"
#include "commons/logger/logger.h"
#include "dblp/index/models/post/index_post.h"
#include "dblp/index/models/fieldtypes/index_element_field_type.h"

class IndexHandler
{
public:
	IndexHandler(QString &indexPath, QString &baseName);

	void load();

	bool findElements(const QString &phrase, IndexElementFieldType field, QSet<quint32> &elements);

	void debug_findArticlesOfAuthor(const QStringList &author);
	void debug_findArticlesOfAuthor(const QString &author);

private:
	static Logger L;

	QString mIndexPath;
	QString mBaseIndexName;

	QFile mKeysFile;
	QTextStream mKeysStream;

	QFile mVocabularyFile;
	QDataStream mVocabularyStream;

	QFile mPostingListFile;
	QDataStream mPostingListStream;

	QFile mElementsPosFile;
	QDataStream mElementsPosStream;

	QList<IndexKey> mKeys;
	QHash<QString, IndexTermRef> mVocabulary;

	void loadKeys();
	void loadVocabulary();

	bool findElements(const QStringList &tokens, IndexElementFieldType field, QSet<quint32> &elements);
	bool findPosts(const QString &term, IndexElementFieldType field, QSet<IndexPost> &post);

	void debug_printKeys();
	void debug_printVocabulary();
};

#endif // INDEX_HANDLER_H
