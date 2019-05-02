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

// Represents a match for findElements(), which contains the element
// id and the term count. The field and the term are implicit with the
// findElements() call
typedef struct ElementFieldMatch {
	quint32 elementId; // element id
	quint8 termCount; // how many times the term occurs within this element.field
} ElementFieldMatch;

class IndexHandler
{
public:
	IndexHandler(QString &indexPath, QString &baseName);

	void load();

	bool findElements(const QString &phrase,
					  IndexElementFieldType fields,
					  QSet<ElementFieldMatch> &matches);

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

	// A QHash would be enough, but since the iefs computation is done
	// scanning the vocabulary, its better if we store the terms in alphabetic
	// order and furthermore we have to keep the posts on the posting list
	// ordered alphabetically by terms.
	// In this way we can have some hope that the computation of the ief
	// uses the cache in a decent way while reading the posting list file.
	QMap<QString, IndexTermRef> mVocabulary;
	QHash<QString, float> mIefs;	// := inverse element frequency
									// log10(|E|/ef_t)

	void loadKeys();
	void loadVocabulary();
	void computeIefs();

	bool findElements(const QStringList &tokens,
					  IndexElementFieldType field,
					  QSet<ElementFieldMatch> &matches);

	// Pass through the vocabulary
	bool findPosts(const QString &term,
				   IndexElementFieldType field,
				   QSet<IndexPost> &posts);

	// Use the ref already taken from the vocabulary
	void findPosts(const QString &term,
				   IndexTermRef &ref,
				   IndexElementFieldType field,
				   QSet<IndexPost> &posts);

	void debug_printKeys();
	void debug_printVocabulary();
	void debug_printIefs();
};

#endif // INDEX_HANDLER_H
