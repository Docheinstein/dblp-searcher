#ifndef INDEXER_H
#define INDEXER_H

#include <QString>
#include <QXmlContentHandler>
#include <commons/file/filestream/data/data_stream_file.h>
#include <commons/file/filestream/text/text_stream_file.h>
#include <commons/log/loggable/loggable.h>
#include <dblp/index/models/entities/index_entities.h>
#include "dblp/index/models/term/index_term.h"
//#include "dblp/xml/models/entities/xml_entities.h"
//#include "dblp/xml/handler/xml_parse_handler.h"
#include "dblp/xml/handler/dblp_xml_parse_handler.h"
#include "dblp/shared/defs/defs.h"
#include <QtGlobal>

class Indexer : public DblpXmlParseHandler,  protected Loggable
{
public:
	Indexer(const QString &indexPath, const QString &baseName);

	// DblpXmlParseHandler interface
	void onParseStart() override;
	void onParseEnd() override;
	bool onElement(const DblpXmlElement &element, qint64 pos) override;

protected:
	LOGGING_OVERRIDE

private:
	// ===================
	// PATHS
	// ===================

	// Folder that will contain the indexes
	QString mOutputPath;

	// Base name assign to each each index file
	// The only thing that changes between index files is the extension.
	QString mBaseIndexName;

	// ===================
	// INDEX FILES STREAMS
	// ===================

	/* Contains the identifiers of each element separated by \n.
	 * The identifier of almost every element is the attribute "key", except
	 * for the journals which are treated as element with an identifier too; for
	 * the journals the identifier is the <journal> tag content in the <article>
	 * fields.
	 * The elements serials are implicits and for each identifier is the line
	 * number, starting from 0).
	 *
	 * e.g.
	 * reference/bdt/2019
	 * phd/Sack2002
	 *
	 * File extension: .idix
	 */
	TextStreamFile mIdentifiersStream;


	/* Contains the posting list, which actually is just a succession
	 * of <element_id><field_number><in_field_term_pos> for each term of each field
	 * of each element of the parsed dblp dump.
	 * The order of the terms actually doesn't matter since those are referred
	 * by the vocabulary.
	 *
	 * <element_id> := 23 bit
	 * <field_number> := 9 bit
	 * <in_field_term_pos> := 8 bit
	 *
	 * File extension: .plix
	 */
	DataStreamFile mPostingsStream;


	/* Contains the original positions of the elements in the dblp.xml file.
	 * The file is a succession of <position>, each refers to the element id
	 * equals to the order in which occur in the file.
	 *
	 * <position> := 32 bit
	 *
	 * e.g
	 * <pos_el_0><pos_el_1><pos_el_2>
	 *
	 * File extension: .epix
	 */
	DataStreamFile mElementsPositionsStream;


	/* Contains the vocabulary, which is a succession of the zero terminated
	 * term and the number of occurences in the posting list for each combination
	 * of element and field.
	 *
	 * A single vocabulary term pattern is something like this:
	 * <term>\0<posting_list_starting_pos><# article.author> <# article.title> <# article.year>...
	 *
	 * The complete element.field list is the following:
	 * <art.a> <art.t> <art.y>
	 * <inc.a> <inc.t> <inc.y> // <inc.b>
	 * <inp.a> <inp.t> <inp.y> // <inp.b>
	 * <phd.a> <phd.t> <phd.y>
	 * <mas.a> <mas.t> <mas.y>
	 * <bok.a> <bok.t> <bok.y> <bok.p>
	 * <pro.t> <pro.y> <pro.p> // <pro.b>
	 *
	 * The size of each element.field is variable, if the count is lower than
	 * 2^15 it will be 16 bit, otherwise it will be 32 with the lefter most
	 * bit set to 1.
	 *
	 * File extension: .vix
	 */
	DataStreamFile mVocabularyStream;


	/* Contains the crossrefs between the publications and the related venues
	 * as a succession of <publication_id><venue_id>.
	 *
	 * The available crossref are:
	 *  incollection => book
	 *  inproceedings => proceedings
	 *  (article => journal)
	 *
	 * The articles unluckily don't contain any crossref since the journals
	 * are not root elements; for this reason we treat the journals as elements
	 * too and we give them an identifier, just like "key" for the elements,
	 * which is actually the journal name; in this way we can keep this dummy
	 * crossref inside this index file too.
	 *
	 * File extension: .cix
	 */
	DataStreamFile mCrossrefsStream;


	// ===================
	// DATA STRUCTURES
	// ===================

	/* Associates terms to "index term entity".
	 * An hash would be sufficient, btw using a map is helpful for compute
	 * additional stuff on the posting list using the terms alphabetical
	 * order (such as compute the iefs of the terms).
	 */
	QMap<QString, IndexTerm> mIndexTerms;

	/* Associates publications element id with the respective venue identifier,
	 * which usually is the key, but might even by another field (e.g. for
	 * the journals is the <journal> field inside <article>.
	 */
	QHash<elem_serial, QString> mVenueIdentifierByPublicationSerial;

	/* Associates venues identifiers with the respective venue element id. */
	QHash<QString, elem_serial> mVenueSerialByVenueIdentifier;

	/* List of the identifiers in the order in which they have been scanned;
	 * Each identifier has its own implicit element id which is just the position
	 * in which it occurs in the list.
	 * Identifier is usually just a synonim of "key" attribute, except for
	 * the journals for which the journal name is the identifier (a journal
	 * element doesnt exists in dblp.xml)
	 */
	QList<QString> mIdentifiers;

	/* Parallel list of mIdentifiers which contains the positions of the elements
	 * in the original dblp.xml file (an approximation of the position: an upper
	 * bound).
	 */
	QList<qint64> mPositions;

	// ===================
	// INDEXING/PARSING STATE
	// ===================

	elem_serial mCurrentSerial = 0;


	// XML INTERNAL SHORTCUTS

	void handleArticle(const DblpArticle &article, qint64 pos);
	void handleJournal(const DblpJournal &journal, qint64 pos);
	void handleIncollection(const DblpIncollection &incollection, qint64 pos);
	void handleBook(const DblpBook &book, qint64 pos);
	void handleInproceedings(const DblpInproceedings &inproc, qint64 pos);
	void handleProceedings(const DblpProceedings &proc, qint64 pos);
	void handlePhdThesis(const DblpPhdThesis &phd, qint64 pos);
	void handleMasterThesis(const DblpMasterThesis &master, qint64 pos);

	// DATA STRUCTURES ADDITIONS

	// To mIndexTerms

	void addTermsOfFields(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
				   const QVector<QString> &fieldsContent);
	void addTermsOfField(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
				  const QString &fieldContent, field_num fieldNumber = 0);
	void addTerm(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
				 const QString &term, field_num fieldNumber, term_pos termPosition);

	void addIdentifier(const QString &key); // to mIdentifiers
	void addPosition(qint64 pos); // to mPositions

	void addCrossref(const QString &externalVenueIdentifier); // to mVenueIdentifierByPublicationSerial
	void addVenue(const QString &venueIdentifier); // to mVenueSerialByVenueIdentifier

	// MISC

	void elementHandled();

	// INDEX FILES CREATIONS

	void writeIdentifiersFile();

	void writePostingListAndVocabularyFiles();
	void writeTermMetas(const QString &term); // to vocabulary
	void writeTermFieldPostsCount(quint32 count); // to vocabulary

	void writePosts(const IndexPosts &posts);
	void writePost(const IndexPost &post);

	void writePositionsFile();

	void writeCrossrefsFile();

	// ===================
	// STATS/DEBUG
	// ===================

	struct {
		quint64 postsCount = 0;
		quint32 highestFieldNumber = 0;
		quint32 highestInFieldPosition = 0;
	} mStats;

	void printStats();
};

#endif // INDEXER_H
