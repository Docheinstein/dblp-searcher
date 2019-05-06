#ifndef INDEX_HANDLER_H
#define INDEX_HANDLER_H

#include <QDataStream>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>
#include <commons/log/loggable/loggable.h>
#include "dblp/index/models/key/index_key.h"
#include "dblp/index/models/reference/index_term_ref.h"
#include "dblp/index/models/post/index_post.h"
#include "dblp/shared/element_field_type/element_field_type.h"

// Represents a match for findElements(), which contains the element
// id and the term count. The field and the term are implicit with the
// findElements() call
typedef struct ElementFieldMatch {
	quint32 elementId; // element id
	quint8 matchCount; // how many times the term(s) occur(s) within this element.field
} ElementFieldMatch;

class IndexHandler : public QObject, protected Loggable
{
	Q_OBJECT

public:
	IndexHandler(const QString &indexPath, const QString &baseName);

	void load();

	// Use a phrase, that will be automatically splitted to tokens
	bool findElements(const QString &phrase,
					  ElementFieldTypes fields,
					  QSet<ElementFieldMatch> &matches);

	// Use the token list
	bool findElements(const QStringList &tokens,
					  ElementFieldTypes fields,
					  QSet<ElementFieldMatch> &matches);

	// Pass through the vocabulary
	bool findPosts(const QString &term,
				   ElementFieldType field,
				   QSet<IndexPost> &posts);

	// Use the ref already taken from the vocabulary
	void findPosts(const QString &term,
				   const IndexTermRef &ref,
				   ElementFieldType field,
				   QSet<IndexPost> &posts);

	const QList<IndexKey> keys() const;
	const QMap<QString, IndexTermRef> vocabulary() const;

signals:
	void vocabularyLoadStarted();
	void vocabularyLoadProgress(double progress);
	void vocabularyLoadEnded();

	void keysLoadStarted();
	void keysLoadProgress(double progress);
	void keysLoadEnded();

protected:
	LOGGING_OVERRIDE

private:
	// Use the token list
	bool findElementsSingleType(
						const QStringList &tokens,
						ElementFieldType field,
						QSet<ElementFieldMatch> &matches);
	void init();

	// Exposed so that the call may have a better control over the status
	// of the loading, if he/she wants
	void loadKeys();
	void loadVocabulary();

	void debug_printKeys();
	void debug_printVocabulary();

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

	// Contains all the elements keys
	QList<IndexKey> mKeys;

	// A QHash would be enough, but since the iefs computation is done
	// scanning the vocabulary, its better if we store the terms in alphabetic
	// order and furthermore we have to keep the posts on the posting list
	// ordered alphabetically by terms.
	// In this way we can have some hope that the computation of the ief
	// uses the cache in a decent way while reading the posting list file.
	QMap<QString, IndexTermRef> mVocabulary;
};

#endif // INDEX_HANDLER_H
