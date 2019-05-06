#ifndef INDEXER_H
#define INDEXER_H

#include <QString>
#include <QXmlContentHandler>
#include <commons/log/loggable/loggable.h>
#include "dblp/index/models/term/index_term.h"
#include "dblp/xml/models/entities/xml_entities.h"
#include "dblp/xml/handler/xml_parse_handler.h"

class Indexer : public XmlParseHandler, protected Loggable
{
public:
	Indexer(const QString &indexPath, const QString &baseName);

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

protected:
	LOGGING_OVERRIDE

private:
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

	// File that contains the original position of the elements in the dblp.xml
	// file. Each position is 32 and refers to the element id equals to the order
	// in which occur in the file.
	QFile mElementsPosFile;
	QDataStream mElementsPosStream;

	// File that contains the vocabulary, which is a succession of zero terminated
	// term and the number of occurences in the posting list for each combination
	// of element and field
	// <term>\0<#aa><#at><#ay>...

	QFile mVocabularyFile;
	QDataStream mVocabularyStream;

	// Associated terms to "index term entity"
	// An hash would be sufficient, btw using a map is helpful for compute
	// additional stuff on the posting list using the terms alphabetical
	// order (such as compute the iefs of the terms)
	QMap<QString, IndexTerm> mIndexTerms;

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

	void addFields(void (*termAdder)(IndexTerm &, const IndexPost &),
				   QList<QString> &fieldsContent);
	void addField(void (*termAdder)(IndexTerm &, const IndexPost &),
				  const QString &fieldContent, quint32 fieldNumber = 0);

	void addTerm(void (*termAdder)(IndexTerm &, const IndexPost &),
				 const QString &term, quint32 fieldNumber, quint32 termPosition);

	void writeElement(const QString &key, qint64 pos);
	void writeKey(const QString &key);
	void writeElementPosition(qint64 pos);

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
