#ifndef INDEX_HANDLER_H
#define INDEX_HANDLER_H

#include <QDataStream>
#include <QFile>
#include <QHash>
#include <QString>
#include <QTextStream>
#include <commons/file/filestream/data/data_stream_file.h>
#include <commons/file/filestream/text/text_stream_file.h>
#include <commons/log/loggable/loggable.h>
#include "dblp/index/models/key/index_key.h"
#include "dblp/index/models/reference/index_term_ref.h"
#include "dblp/index/models/post/index_post.h"
#include "dblp/shared/element_field_type/element_field_type.h"
#include "commons/profiler/profiler.h"
#include "dblp/index/models/match/index_match.h"


class IndexHandler : public QObject, protected Loggable
{
	Q_OBJECT

public:
	IndexHandler(const QString &indexPath, const QString &baseName);

	void load();

	// Use a phrase, that will be automatically splitted to tokens
	bool findMatches(const QString &phrase,
					  ElementFieldTypes fieldTypes,
					 QSet<IndexMatch> &matches);

	// Use the token list
	bool findMatches(const QStringList &tokens,
					ElementFieldTypes fieldTypes,
					 QSet<IndexMatch> &matches);


	bool findWordMatches(
						const QString &token,
						ElementFieldType fieldType,
						QSet<IndexMatch> &matches);


	bool findPhraseMatches(
						const QStringList &tokens,
						ElementFieldType fieldType,
						QSet<IndexMatch> &matches);


	// Pass through the vocabulary
	bool findPosts(const QString &term,
				   ElementFieldType fieldType,
				   QSet<IndexPost> &posts);

	// Use the ref already taken from the vocabulary
	void findPosts(const QMap<QString, IndexTermRef>::const_iterator vocabularyEntry,
				   ElementFieldType fieldType,
				   QSet<IndexPost> &posts);

	const QList<QString> identifiers() const;
	bool identifier(elem_serial serial, QString &identifier) const;

	const QMap<QString, IndexTermRef> vocabulary() const;
	bool vocabularyTermRef(const QString &term, IndexTermRef &termRef) const;

	const QHash<elem_serial, elem_serial> crossrefs() const;
	bool crossref(elem_serial publicationSerial, elem_serial &venueSerial) const;

signals:
	void vocabularyLoadStarted();
	void vocabularyLoadProgress(double progress);
	void vocabularyLoadEnded();

	void keysLoadStarted();
	void keysLoadProgress(double progress);
	void keysLoadEnded();

	void crossrefsLoadStarted();
	void crossrefsLoadProgress(double progress);
	void crossrefsLoadEnded();

//	void articlesJournalsLoadStarted();
//	void articlesJournalsLoadProgress(double progress);
//	void articlesJournalsLoadEnded();

protected:
	LOGGING_OVERRIDE

private:
	// Use the token list
//	bool findMatchesSingleType(
//						const QStringList &tokens,
//						ElementFieldType fieldType,
//						QSet<IndexMatch> &matches);

//	bool findMatchesSingleTypeWord(
//						const QString &token,
//						ElementFieldType fieldType,
//						QSet<IndexMatch> &matches);

//	bool findMatchesSingleTypePhrase(
//						const QStringList &tokens,
//						ElementFieldType fieldType,
//						QSet<IndexMatch> &matches);

	void init();

	void loadIdentifiers();
	void loadVocabulary();
	void loadCrossrefs();
//	void loadPositions();
	// ...

	void printIdentifiers();
	void printVocabulary();
	void printCrossrefs();

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
	DataStreamFile mPostingsStream;
	DataStreamFile mElementsPositionsStream;
	DataStreamFile mCrossrefsStream;

	// ===================
	// DATA STRUCTURES
	// ===================

	// Contains all the elements identifiers (with the implicit association
	// to their serial, which are equals to the in-list position)
	QList<QString> mIdentifiers;

	// A QHash would be enough, but since the iefs computation is done
	// scanning the vocabulary, its better if we store the terms in alphabetic
	// order and furthermore we have to keep the posts on the posting list
	// ordered alphabetically by terms.
	// In this way we can have some hope that the computation of the ief
	// uses the cache in a decent way while reading the posting list file.
	QMap<QString, IndexTermRef> mVocabulary;

	// Contains the crossrefs from publications to venues
	// for inproc => proc, incollection => book, article => journal
	QHash<elem_serial, elem_serial> mCrossrefs;
};

#endif // INDEX_HANDLER_H
