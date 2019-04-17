#ifndef INDEXER_H
#define INDEXER_H

#include <QString>
#include <QXmlContentHandler>
#include <dblp/index/term/index_term.h>
#include "commons/logger/logger.h"
#include "dblp/parse/handler/dblp_xml_parse_handler.h"

class Indexer : public DblpXmlParseHandler
{
public:
	Indexer(QString &indexPath, QString &baseName);

	// DblpXmlParseHandler interface
	void onStart();
	void onEnd();
	void onArticle(DblpArticle &article, qint64 pos);
	void onJournal(DblpJournal &journal, qint64 pos);
	void onIncollection(DblpIncollection &incollection, qint64 pos);
	void onBook(DblpBook &book, qint64 pos);
	void onInproceedings(DblpInproceedings &inproc, qint64 pos);
	void onProceedings(DblpProceedings &proc, qint64 pos);
	void onPhdThesis(DblpPhdThesis &phd, qint64 pos);
	void onMasterThesis(DblpMasterThesis &master, qint64 pos);

private:
	Logger L = Logger::forClass("Indexer");

	// Folder that will contain the indexes
	QString mOutputPath;

	// Base name assign to each each index file
	// The only thing that changes between index files is the extension.
	QString mBaseIndexName;

	// File that contains each scanned key, separated by \n
	// The elements id can be figured out since its exactly the
	// order in which the key occurs in the keys file
	QFile mKeysFile;
	QTextStream mKeysStream;

	// File that contains the posting list, which actually is just a succession
	// of <element><field_number><in_field_term_pos> for each term of each field
	// of each element of the parsed dblp dump.
	// The order of the terms actually doesn't matter since those are referred
	// by the vocabulary
	QFile mPostingListFile;
	QDataStream mPostingListStream;

	// File that contains the vocabulary, which is a succession of zero terminated
	// term and the number of occurences in the posting list for each combination
	// of element and field
	// <term>\0<#aa><#at><#ay>...

	QFile mVocabularyFile;
	QDataStream mVocabularyStream;

	// Associated terms to "index term entity"
	// TODO: probably qmap should be replaced with qhash for performance
	// reason even if the latter doesn't keep the terms order
	QMap<QString, IndexTerm *> mIndexTerms;

	struct {
		// Current element/key id
		quint32 elementId = 0;

		// Current byte position of the indexed element
		// This is not exact, but is an upper bound since the read is done
		// buffered through QTextStream. (The step currently is 1024)
		qint64 pos = 0;

		// Buffer size of the last chunk of data read for the parsing.
		// Is kept and written to the keys file in order to figure out
		// the real position of the keys (which are separated by \n, and
		// each \n means a step of 1024 bytes)
		qint64 readBufferSize = 0;
	} mCurrent;

	void addFields(void (*termAdder)(IndexTerm *, IndexPost),
				   QList<QString> &fieldsContent);
	void addField(void (*termAdder)(IndexTerm *, IndexPost),
				  const QString &fieldContent, quint32 fieldNumber = 0);

	void addTerm(void (*termAdder)(IndexTerm *, IndexPost),
				 const QString &term, quint32 fieldNumber, quint32 termPosition);

	void writeElement(QString &key, qint64 pos);
	void writeKey(QString &key, qint64 pos);

	void createPostingListAndVocabulary();
	void writeVocabularyTermMetas(QString term);
	void writeVocabularyTermFieldPostsCount(quint32 byteOffset);
	void writePostingListTermPosts(const IndexPost &post);
	void writeFieldPosts(QList<IndexPost> &posts);

	quint32 debug_mDebugPostsCount = 0;
	quint32 debug_mHighestFieldNumber = 0;
	quint32 debug_mHighestInFieldPos = 0;
	void debug_printIndexTerms();
	void debug_printIndexStats();
};

#endif // INDEXER_H
