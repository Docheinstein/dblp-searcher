#ifndef INDEX_HANDLER_H
#define INDEX_HANDLER_H

#include <QMap>
#include <QHash>
#include <QString>

#include "commons/file/filestream/data/data_stream_file.h"
#include "commons/file/filestream/text/text_stream_file.h"
#include "commons/log/loggable/loggable.h"
#include "commons/profiler/profiler.h"
#include "dblp/index/models/reference/index_term_ref.h"
#include "dblp/index/models/post/index_post.h"
#include "dblp/shared/element_field_type/element_field_type.h"
#include "dblp/index/models/match/index_match.h"


// Use an hash or a map depending on whether we are computing all the ief
// at once or not.
// A QHash would be enough, but since the iefs computation is done
// scanning the vocabulary, its better if we store the terms in alphabetic
// order and furthermore we have to keep the posts on the posting list
// ordered alphabetically by terms.
// In this way we can have some hope that the computation of the ief
// uses the cache in a decent way while reading the posting list file.
#ifdef LAZY_IEF
#define VOCABULARY_ADT QHash
#else
#define VOCABULARY_ADT QMap
#endif

class IndexHandler : public QObject, protected Loggable
{
	Q_OBJECT

public:
	IndexHandler(const QString &indexPath, const QString &baseName,
				 bool loadPositions = false /* for XML retrieval */);

	void load();


	// ===================
	// RUNTIME INDEX STUFF RETRIEVAL (everything apart from posting list stuff)
	// ===================

	const QVector<QString> identifiers() const;
	bool identifier(elem_serial serial, QString &identifier) const;

	const VOCABULARY_ADT<QString, IndexTermRef> vocabulary() const;
	bool vocabularyTermRef(const QString &term, IndexTermRef &termRef) const;

	const QHash<elem_serial, elem_serial> crossrefs() const;
	bool crossref(elem_serial publicationSerial, elem_serial &venueSerial) const;

	const QHash<elem_serial, QVector<elem_serial>> inverseCrossrefs() const;
	bool inverseCrossref(elem_serial venueSerial, QVector<elem_serial> &publicationSerials) const;

	const QVector<elem_pos> positions() const;
	bool positionRange(elem_serial serial, QPair<elem_pos, elem_pos> &range) const;


	// ===================
	// POSTS FINDERS (which goes to posting list index)
	// ===================


	// All the finds are thread-safe (since findPosts is thread-safe)

	// Use a phrase, that will be automatically splitted to tokens
	bool findMatches(const QString &phrase,
					 ElementFieldTypes fieldTypes,
					 QVector<IndexMatch> &matches);

	// Use the token list
	bool findMatches(const QStringList &tokens,
					ElementFieldTypes fieldTypes,
					QVector<IndexMatch> &matches);


	bool findWordMatches(
						const QString &token,
						ElementFieldType fieldType,
						QVector<IndexMatch> &matches);


	bool findPhraseMatches(
						const QStringList &tokens,
						ElementFieldType fieldType,
						QVector<IndexMatch> &matches);


	// Pass through the vocabulary
	bool findPosts(const QString &term,
				   ElementFieldType fieldType,
				   QVector<IndexPost> &posts);

	// Use the ref already taken from the vocabulary
	void findPosts(const VOCABULARY_ADT<QString, IndexTermRef>::const_iterator vocabularyEntry,
				   ElementFieldType fieldType,
				   QVector<IndexPost> &posts);

signals:
	void vocabularyLoadStarted();
	void vocabularyLoadProgress(double progress);
	void vocabularyLoadEnded();

	void identifiersLoadStarted();
	void identifiersLoadProgress(double progress);
	void identifiersLoadEnded();

	void crossrefsLoadStarted();
	void crossrefsLoadProgress(double progress);
	void crossrefsLoadEnded();

	void positionsLoadStarted();
	void positionsLoadProgress(double progress);
	void positionsLoadEnded();

protected:
	LOGGING_OVERRIDE

private:
	void init();

	void loadIdentifiers();
	void loadVocabulary();
	void loadCrossrefs();
	void loadPositions();

	void printIdentifiers();
	void printVocabulary();
	void printCrossrefs();
	void printPositions();


	// ===================
	// PATHS
	// ===================

	QString mIndexPath;
	QString mBaseIndexName;


	// ===================
	// INDEX FILES STREAMS
	// ===================

	TextStreamFile mIdentifiersStream;
	DataStreamFile mVocabularyStream;
	DataStreamFile mXmlPositionsStream;
	DataStreamFile mCrossrefsStream;
	DataStreamFile mPostingsStream;


	// ===================
	// DATA STRUCTURES
	// ===================

	// Contains all the elements identifiers (with the implicit association
	// to their serial, which are equals to the in-list position)
	QVector<QString> mIdentifiers;

	// Contains the terms and their reference to the post in the posting list.
	VOCABULARY_ADT<QString, IndexTermRef> mVocabulary;

	// Contains the crossrefs from publications to venues
	// for inproc => proc, incollection => book, article => journal
	QHash<elem_serial, elem_serial> mCrossrefs;

	// Contains the inverse crossrefs from venues to publications
	// Doesn't reflect an index file, but is automatically generated during
	// the construction of the direct crossrefs, for convenience
	QHash<elem_serial, QVector<elem_serial>> mInverseCrossrefs;

	// Contains the elements positions in the original XML file, one for each element
	QVector<elem_pos> mXmlPositions;


	// ====================
	// MISC
	// ====================

	// Whether we have to handle position file (e.g. for retrieval of original
	// elements from XML file)
	bool mLoadPositionsFeature;
};

#endif // INDEX_HANDLER_H
