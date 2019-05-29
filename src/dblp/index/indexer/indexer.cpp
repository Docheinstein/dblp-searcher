#include "indexer.h"

#include <QDir>

#include "commons/config/app_config.h"
#include "commons/const/const.h"
#include "commons/config/config.h"
#include "commons/util/util.h"
#include "commons/globals/globals.h"
#include "commons/profiler/profiler.h"

LOGGING(Indexer, true)

namespace DblpXmlElements = Const::Dblp::Xml::Elements;
namespace DblpXmlFields = Const::Dblp::Xml::Fields;
namespace DblpXmlAttributes = Const::Dblp::Xml::Attributes;

Indexer::Indexer(const QString &outputPath, const QString &baseName)
{
	mOutputPath = outputPath;
	mBaseIndexName = baseName;
}

void Indexer::onParseStart()
{
	static const char * const INDEX_CREATION_ERROR =
			"Cannot create index file directory. "
			"Is the path valid? Do you have permissions?";

	// Open files

	if (!QDir::root().mkpath(mOutputPath)) {
		QUIT(INDEX_CREATION_ERROR);
	}

	// Identifiers file
	QString identifiersPath = Util::Dblp::Index::indexFilePath(
			mOutputPath, mBaseIndexName, Config::Index::Extensions::IDENTIFIERS);
	ii("Creating identifiers index file at: " << identifiersPath);
	if (!mIdentifiersStream.openWrite(identifiersPath))
		QUIT(INDEX_CREATION_ERROR);

	// Posting list file
	QString postingListPath = Util::Dblp::Index::indexFilePath(
			mOutputPath, mBaseIndexName, Config::Index::Extensions::POSTING_LIST);
	ii("Creating posting list index file at: " << postingListPath);
	if (!mPostingsStream.openWrite(postingListPath))
		QUIT(INDEX_CREATION_ERROR);

	// Vocabulary file
	QString vocabularyPath = Util::Dblp::Index::indexFilePath(
			mOutputPath, mBaseIndexName, Config::Index::Extensions::VOCABULARY);
	ii("Creating vocabulary index file at: " << vocabularyPath);
	if (!mVocabularyStream.openWrite(vocabularyPath))
		QUIT(INDEX_CREATION_ERROR);

	// Elements positions file
	QString xmlPosPath = Util::Dblp::Index::indexFilePath(
			mOutputPath, mBaseIndexName, Config::Index::Extensions::XML_POS);
	ii("Creating elements pos index file at: " << xmlPosPath);
	if (!mXmlPositionsStream.openWrite(xmlPosPath))
		QUIT(INDEX_CREATION_ERROR);

	// Crossrefs file
	QString crossrefsPath = Util::File::path(
		{mOutputPath, mBaseIndexName + Config::Index::Extensions::CROSSREFS});
	ii("Creating crossrefs index file at: " << crossrefsPath);
	if (!mCrossrefsStream.openWrite(crossrefsPath))
		QUIT(INDEX_CREATION_ERROR);

	ii("Started parsing of XML file...");

	// Start the timer
	mStats.timer.start();
}

void Indexer::onParseEnd()
{
	ii("Finished parsing of XML file");
	ii("Actually writing index files...");

	vv("Writing index files");

	writeIdentifiersFile();
	writePostingListAndVocabularyFiles();
	writePositionsFile();
	writeCrossrefsFile();

	vv("Closing index files");

	mIdentifiersStream.close();
	mVocabularyStream.close();
	mPostingsStream.close();
	mXmlPositionsStream.close();
	mCrossrefsStream.close();

	ii("Indexing finished");

	printStats();
}


bool Indexer::onElement(const DblpXmlElement &element, qint64 pos)
{
	PROF_FUNC_BEGIN

	dd("On element: " << endl << QString(element));

	// Create the right dblp entity accordingly to the xml element
	// and deliver it to the right handler

	auto fillWork = [](DblpWork &work, const DblpXmlElement xmlElement) {
		work.key = xmlElement.attributes.value(DblpXmlAttributes::KEY).toString();
		work.title = xmlElement.fields.value(DblpXmlFields::TITLE, {""}).at(0);
		work.year = xmlElement.fields.value(DblpXmlFields::YEAR, {""}).at(0);
	};

	auto fillPublication = [&fillWork](
			DblpPublication &pub, const DblpXmlElement &xmlElement) {
		fillWork(pub, xmlElement);
		pub.authors = xmlElement.fields.value(DblpXmlFields::AUTHOR);
	};

	auto fillPublicationCrossref = [&fillPublication](
			DblpPublicationCrossref &pub, const DblpXmlElement &xmlElement) {
		fillPublication(pub, xmlElement);
		pub.crossref = xmlElement.fields.value(DblpXmlFields::CROSSREF, {""}).at(0);
	};

	auto fillVenue = [&fillWork](DblpVenue &venue, const DblpXmlElement &xmlElement) {
		fillWork(venue, xmlElement);
		venue.publisher = xmlElement.fields.value(DblpXmlFields::PUBLISHER, {""}).at(0);
	};

	if (element.name == DblpXmlElements::ARTICLE) {
		DblpJournal journal;
		journal.name = element.fields.value(DblpXmlFields::JOURNAL, {""}).at(0);
		handleJournal(journal, pos);

		DblpArticle article;
		fillPublication(article, element);
		article.journal = journal.name;
		handleArticle(article, pos);
	} else if (element.name == DblpXmlElements::INCOLLECTION) {
		DblpIncollection incollection;
		fillPublicationCrossref(incollection, element);
		handleIncollection(incollection, pos);
	} else if (element.name == DblpXmlElements::BOOK) {
		DblpBook book;
		fillVenue(book, element);
		handleBook(book, pos);
	} else if (element.name == DblpXmlElements::INPROCEEDINGS) {
		DblpInproceedings inproc;
		fillPublicationCrossref(inproc, element);
		handleInproceedings(inproc, pos);
	} else if (element.name == DblpXmlElements::PROCEEDINGS) {
		DblpProceedings proc;
		fillVenue(proc, element);
		handleProceedings(proc, pos);
	} else if (element.name == DblpXmlElements::PHDTHESIS) {
		DblpPhdThesis phd;
		fillPublication(phd, element);
		handlePhdThesis(phd, pos);
	} else if (element.name == DblpXmlElements::MASTERSTHESIS) {
		DblpMastersThesis master;
		fillPublication(master, element);
		handleMastersThesis(master, pos);
	}

	PROF_FUNC_END

	return true; // always go ahead to the rescue!
}

void Indexer::handleArticle(const DblpArticle &article, qint64 pos)
{
	vv("Handling article: " << article);

	vv1("Adding terms of article.authors");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.article.author.append(post); },
		article.authors
	);

	vv1("Adding terms of article.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.article.title.append(post); },
		article.title
	);

	vv1("Adding terms of article.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.article.year.append(post); },
		article.year
	);

	addIdentifier(article.key);
	addPosition(pos);
	addCrossref(article.journal);
	elementHandled();
}

void Indexer::handleJournal(const DblpJournal &journal, qint64 pos)
{
	vv("Handling journal: " << journal.name);

	// Unluckily we cannot add the journal name as identifier since onJournal
	// is triggered for each <journal> match inside the articles
	// Instead we have to check if it has already been added
	// (to mVenueSerialByVenueIdentifier); if not we can handle it as any
	// other element, otherwise we have to skip it.

	auto journalIdentifierIt = mVenueSerialByVenueIdentifier.constFind(journal.name);

	if (journalIdentifierIt != mVenueSerialByVenueIdentifier.cend()) {
		dd1("Skipping journal addition since already exists: " << journal.name);
		// Already added, do not add again
		return;
	}

	// Treat it as any other element

	vv1("Adding journal for the first time: " << journal.name);

	vv1("Adding terms of journal");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.journal.name.append(post); },
		journal.name
	);

	// This journal has never been added, add it for the first time as
	// a new (and unique) identifier
	addIdentifier(journal.name);
	addPosition(pos); // this actually refers to the position of the
					  // article that embeds this journal
	addVenue(journal.name);
	elementHandled();
}

void Indexer::handleIncollection(const DblpIncollection &incollection, qint64 pos)
{
	vv("Handling incollection: " << incollection);

	vv1("Adding terms of incollection.author");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.author.append(post); },
		incollection.authors
	);

	vv1("Adding terms of incollection.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.title.append(post); },
		incollection.title
	);

	vv1("Adding terms of incollection.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.year.append(post); },
		incollection.year
	);

	addIdentifier(incollection.key);
	addPosition(pos);
	addCrossref(incollection.crossref);
	elementHandled();
}

void Indexer::handleBook(const DblpBook &book, qint64 pos)
{
	vv("Handling book: " << book);

	vv1("Adding terms of book.authors");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.book.author.append(post); },
		book.authors
	);

	vv1("Adding terms of book.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.book.title.append(post); },
		book.title
	);

	vv1("Adding terms of book.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.book.year.append(post); },
		book.year
	);

	vv1("Adding terms of book.publisher");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.book.publisher.append(post); },
		book.publisher
	);

	addIdentifier(book.key);
	addPosition(pos);
	addVenue(book.key);
	elementHandled();
}

void Indexer::handleInproceedings(const DblpInproceedings &inproc, qint64 pos)
{
	vv("Handling inproceedings: " << inproc);

	vv1("Adding terms of inproc.authors");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.author.append(post); },
		inproc.authors
	);

	vv1("Adding terms of inproc.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.title.append(post); },
		inproc.title
	);

	vv1("Adding terms of inproc.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.year.append(post); },
		inproc.year
	);

	addIdentifier(inproc.key);
	addPosition(pos);
	addCrossref(inproc.crossref);
	elementHandled();
}

void Indexer::handleProceedings(const DblpProceedings &proc, qint64 pos)
{
	vv("Handling proceedings: " << proc);

	vv1("Adding terms of proc.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.proceedings.title.append(post); },
		proc.title
	);

	vv1("Adding terms of proc.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.proceedings.year.append(post); },
		proc.year
	);

	addIdentifier(proc.key);
	addPosition(pos);
	addVenue(proc.key);
	elementHandled();
}

void Indexer::handlePhdThesis(const DblpPhdThesis &phd, qint64 pos)
{
	vv("Handling phdthesis: " << phd);

	vv1("Adding terms of phd.authors");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.author.append(post); },
		phd.authors
	);

	vv1("Adding terms of phd.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.title.append(post); },
		phd.title
	);

	vv1("Adding terms of phd.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.year.append(post); },
		phd.year
	);

	addIdentifier(phd.key);
	addPosition(pos);
	elementHandled();
}

void Indexer::handleMastersThesis(const DblpMastersThesis &master, qint64 pos)
{
	dd("Handling mastersthesis: " << master);

	vv1("Adding terms of master.authors");
	addTermsOfFields([](IndexTerm &term, const IndexPost &post)
		{ term.mastersthesis.author.append(post); },
		master.authors
	);

	vv1("Adding terms of master.title");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.mastersthesis.title.append(post); },
		master.title
	);

	vv1("Adding terms of master.year");
	addTermsOfField([](IndexTerm &term, const IndexPost &post)
		{ term.mastersthesis.year.append(post); },
		master.year
	);

	addIdentifier(master.key);
	addPosition(pos);
	elementHandled();
}

// For each term inside a field among the fields of the dblp entity we
// have to insert a "post" inside the map (indexed with the term as key)
// that keep tracks of the current element serial, the field number (necessary
// for distinguish multiple field (e.g. <author>), and the position
// of the term within the field.
void Indexer::addTermsOfFields(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
							   const QVector<QString> &fieldsContent)
{
	int fieldsCount = fieldsContent.size();

	ASSERT(UINT32(fieldsCount) < Config::Index::PostingList::FIELD_NUM_THRESHOLD, "indexing",
		   "Found an element with more fields than the index's allowed number");

	for (int i = 0; i < fieldsCount; ++i) {
		const QString &fieldContent = fieldsContent.at(i);
		addTermsOfField(indexTermPostAdder, fieldContent, static_cast<field_num>(i));
	}
}

void Indexer::addTermsOfField(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
							  const QString &fieldContent, field_num fieldNumber)
{
	vv2("Adding field: '" << fieldContent << "' ; field number: " << fieldNumber);

	QStringList terms = fieldContent.split(" ", QString::SplitBehavior::SkipEmptyParts);

	int termsCount = terms.size();
	ASSERT(UINT32(termsCount) < Config::Index::PostingList::IN_FIELD_POS_THRESHOLD,
			 "indexing", "Found a field with more terms than the index's allowed number");

	for (int i = 0; i < termsCount; ++i) {
		const QString &term = terms.at(i);
		vv3("Adding term: '" << term << "'");
		addTerm(indexTermPostAdder, term, fieldNumber, static_cast<term_pos>(i));
	}
}

void Indexer::addTerm(void (*indexTermPostAdder)(IndexTerm &, const IndexPost &),
					  const QString &term,
					  field_num fieldNumber,
					  term_pos termPosition)
{
	// Sanitize term (lowercase, no punctuation, trim, ...)
	QString indexTerm = Util::String::sanitizeTerm(term);

	// Skip the term if it is empty
	// (e.g. if it was a punctuation, and has been sanitized)
	if (indexTerm.isEmpty())
		return;

	if (!mIndexTerms.contains(indexTerm)) {
		// Insert a new index term entity, empty for now
		mIndexTerms.insert(indexTerm, {});
	}

	// Whether we have inserted a new index term or the map
	// already contained it, termEntity now contains the
	// inserted term entity

	IndexTerm &termEntity = *mIndexTerms.find(indexTerm);

	IndexPost post;
	post.elementSerial = mCurrentSerial;
	post.fieldNumber = fieldNumber;
	post.inFieldTermPosition = termPosition;

	vv3("Adding post for term '" << indexTerm << "' : " << post);

	// Add the post to the dblp entity as wanted by the caller
	indexTermPostAdder(termEntity, post);

	termEntity.stats.postsCount++;
}

void Indexer::addIdentifier(const QString &key)
{
	ASSERT(mIdentifiers.size() == INT(mCurrentSerial), "indexing",
			   "Unaligned position addition; mismatch with current serial");

	dd1("Adding element with serial = " << mIdentifiers.size());
	mIdentifiers.append(key);
}

void Indexer::addPosition(qint64 pos)
{
	ASSERT(mPositions.size() == INT(mCurrentSerial), "indexing",
			   "Unaligned position addition; mismatch with current serial");

	ASSERT(pos < Config::Index::ElementsPosition::ELEMENT_POS_THRESHOLD,
			 "indexing", "The original element has a file position above the allowed one");

	dd1("Adding dblp.xml pos for serial = " << mPositions.size() << ": " << pos);

	// No more than 4GB file is allowed
	mPositions.append(static_cast<elem_pos>(pos));
}

void Indexer::addCrossref(const QString &externalVenueIdentifier)
{
	dd1("Adding crossref: " << mCurrentSerial << " => " << externalVenueIdentifier);
	mVenueIdentifierByPublicationSerial.insert(mCurrentSerial, externalVenueIdentifier);
}

void Indexer::addVenue(const QString &venueIdentifier)
{
	dd1("Adding venue: " << venueIdentifier << " => " << mCurrentSerial);
	mVenueSerialByVenueIdentifier.insert(venueIdentifier, mCurrentSerial);
}

void Indexer::elementHandled()
{
	// Advance the serial
	mCurrentSerial++;
}

void Indexer::writeIdentifiersFile()
{
	vv1("Writing identifiers index file ("
		<< mIdentifiers.size() << " elements)");

	elem_serial i = 0;
	for (const QString &key : mIdentifiers) {
		vv2("Writing identifier for element = " << i << ": " << key);
		mIdentifiersStream.stream << key << '\n';
		++i;
	}
}

void Indexer::writePostingListAndVocabularyFiles()
{
	vv1("Writing vocabulary and posting list index files ("
		<< mIndexTerms.size() << " terms)");

	for (auto it = mIndexTerms.cbegin(); it != mIndexTerms.cend(); ++it) {
			const QString &term = it.key();
			const IndexTerm &termEntity = it.value();

			auto writeField = [this](const IndexPosts &posts, bool multifield = false) {
				this->writePosts(posts, multifield);
				this->writeTermFieldPostsCount(UINT32(posts.size()));
			};

			vv2("Handling term: " << term);

			// Write term metas: (term as string and term's starting posting list position)
			writeTermMetas(term);

			// Write the term posts to the posting list,
			// one for each field of the elements

			// * <art.a> <art.t> <art.y>
			// * <jou>
			// * <inc.a> <inc.t> <inc.y>
			// * <inp.a> <inp.t> <inp.y>
			// * <phd.a> <phd.t> <phd.y>
			// * <mas.a> <mas.t> <mas.y>
			// * <bok.a> <bok.t> <bok.y> <bok.p>
			// * <pro.t> <pro.y> <pro.p>

			vv3("Writing posts for term: " << term);

			// article
			// - author
			// - title
			// - year

			writeField(termEntity.article.author, true);
			writeField(termEntity.article.title);
			writeField(termEntity.article.year);

			// journal

			writeField(termEntity.journal.name);

			// incollection
			// - author
			// - title
			// - year

			writeField(termEntity.incollection.author, true);
			writeField(termEntity.incollection.title);
			writeField(termEntity.incollection.year);

			// inproceedings
			// - author
			// - title
			// - year

			writeField(termEntity.inproceedings.author, true);
			writeField(termEntity.inproceedings.title);
			writeField(termEntity.inproceedings.year);

			// phdthesis
			// - author
			// - title
			// - year

			writeField(termEntity.phdthesis.author, true);
			writeField(termEntity.phdthesis.title);
			writeField(termEntity.phdthesis.year);

			// masterthesis
			// - author
			// - title
			// - year

			writeField(termEntity.mastersthesis.author, true);
			writeField(termEntity.mastersthesis.title);
			writeField(termEntity.mastersthesis.year);

			// book
			// - author
			// - title
			// - year
			// - publisher

			writeField(termEntity.book.author, true);
			writeField(termEntity.book.title);
			writeField(termEntity.book.year);
			writeField(termEntity.book.publisher);

			// proceedings
			// - title
			// - year
			// - publisher

			writeField(termEntity.proceedings.title);
			writeField(termEntity.proceedings.year);
			writeField(termEntity.proceedings.publisher);
		}
}

void Indexer::writeTermMetas(const QString &term)
{
	vv3("Writing metas to vocabulary for term: " << term);
	dd4(".vix position: " << mVocabularyStream.filePosition());
	dd4(".plix position: " << mPostingsStream.filePosition());

	// First of all, write the term to the vocabulary

	mVocabularyStream.stream << term.toUtf8();

	// Then write the starting position of the term in the posting list

	mVocabularyStream.stream << mPostingsStream.filePosition();
}

void Indexer::writeTermFieldPostsCount(quint32 count)
{
	// Just append the post count of the field referred to the starting position
	// in the posting list of the term

	// If the count is below REF_SHRINKED_THRESHOLD (32767 = 2^15 - 1)
	// then write a quint16 with the first bit at 0, otherwise write a full quint32
	// with the first bit at 1 (the loader must be aware of the fact that the full
	// quint32 will have the first bit at 1, which should be removed for take the
	// right number)

	if (count < Config::Index::Vocabulary::REF_SHRINKED_THRESHOLD) {
		// 16 bit

		if (count > 0) {
			dd4("Writing posts count: " << UINT16(count));
		}

		mVocabularyStream.stream << UINT16(count);
	}
	else {
		ASSERT(mCurrentSerial < Config::Index::Vocabulary::REF_EXTENDED_THRESHOLD,
				 "indexing", "There are more posts than the index's allowed number");
		// 32 bit, with leftmost bit = 1

		if (count > 0) {
			dd4("Writing posts count: " << count);
		}

		mVocabularyStream.stream <<
			// In order to distinguish this from the < 16 bit case, add a
			// flag (1) to the left of the byte
			(count | Config::Index::Vocabulary::REF_EXTENDED_FLAG);

		++mStats.largeTermRefCount;
	}

	++mStats.termRefCount;
}

void Indexer::writePosts(const IndexPosts &posts, bool multifield)
{
	// Write the posts to the posting list

	for (const IndexPost & post : posts) {
		dd4("Writing post: " << post);
		writePost(post, multifield);
	}
}

void Indexer::writePost(const IndexPost &post, bool multifield)
{
	dd4("Writing post: " << post);


	dd("-> multifield: " << multifield);
	// Asserts
	ASSERT(post.elementSerial < Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD,
			 "indexing", "There are more elements than the index's allowed number");

	ASSERT(post.fieldNumber < Config::Index::PostingList::FIELD_NUM_THRESHOLD,
			"indexing", "There are more equals fields per element than the index's allowed number");

	// Implicit since inFieldTermPosition type is exactly the same size of IN_FIELD_POS_THRESHOLD
//	ASSERT(post.inFieldTermPosition < Config::Index::PostingList::IN_FIELD_POS_THRESHOLD,
//			"indexing", "There are more terms per field than the index's allowed number");

	if (multifield) {
		quint32 P32 =  UINT32(
				(post.elementSerial << Config::Index::PostingList::FIELD_NUM_BITS) |
				post.fieldNumber
		);

		quint8 P8 = post.inFieldTermPosition;

		dd5("Writing P32 to posting list Hex: " << HEX(P32));
		dd5("Writing P8 to posting list Hex: " << HEX(P8));

		mPostingsStream.stream << P32 << P8;
	} else {
		quint32 P32 = UINT32(
			(post.elementSerial << Config::Index::PostingList::IN_FIELD_POS_BITS) |
			post.inFieldTermPosition
		);

		dd5("Writing P32 to posting list Hex: " << HEX(P32));

		mPostingsStream.stream << P32;

		// Implicit...
		ASSERT((P32 & 0x80000000) == 0, "indexing",
			   "Left padding bit for monofield elements should be 0");

		ASSERT((post.inFieldTermPosition & 0xFF) == post.inFieldTermPosition, "indexing",
			   "inFieldTermPosition took more than 8 bits");

	}

	dd5("New .plix buffer pos: " << mPostingsStream.filePosition());

	// Stats

	mStats.postsCount++;

	if (post.inFieldTermPosition > mStats.highestInFieldPosition)
		mStats.highestInFieldPosition = post.inFieldTermPosition;

	if (post.fieldNumber > mStats.highestFieldNumber)
		mStats.highestFieldNumber = post.fieldNumber;
}

void Indexer::writePositionsFile()
{
	vv1("Writing positions index file");

	elem_serial i = 0;

	for (elem_pos pos : mPositions) {
		vv2("Writing position for element = " << i << ": " << pos);

		mXmlPositionsStream.stream << pos;

		++i;
	}
}

void Indexer::writeCrossrefsFile()
{
	vv1("Writing crossrefs index file");

	// The format is <pub_element_id><venue_element_id>
	// But we can't do it directly, we have to do a double lookup

	for (auto it = mVenueIdentifierByPublicationSerial.cbegin();
		 it != mVenueIdentifierByPublicationSerial.cend();
		 ++it) {

		// For each publication element id retrieve the element
		// id of the venue
		elem_serial publicationSerial = it.key();
		const QString &venueIdentifier = it.value();

		auto venueSerialIt = mVenueSerialByVenueIdentifier.constFind(venueIdentifier);

		if (venueSerialIt == mVenueSerialByVenueIdentifier.cend()) {
			ww("Could not find the element id of the crossreffed venue: " << venueIdentifier);
			continue; // ignore it...
		}

		elem_serial venueSerial = venueSerialIt.value();

		vv2("Writing crossref " << publicationSerial << " => " << venueSerial);

		// We have everything, really write this pair of <pub> => <venue>
		// Since each serial is no more than 23 bit, write those using a 48 bit
		// with the first two left bit 0 padded
		static const quint32 CX_LSH = // 7
				(32 - Config::Index::PostingList::ELEMENT_SERIAL_BITS) -
				(48 - (Config::Index::PostingList::ELEMENT_SERIAL_BITS << 1));

		static const quint32 CX_RSH = // 16
				(32 - (Config::Index::PostingList::ELEMENT_SERIAL_BITS - CX_LSH));

		quint32 C32 = ((publicationSerial << CX_LSH) | (venueSerial >> CX_RSH));
		quint16 C16 = venueSerial & 0xFFFF; // Last 16

		dd3("Writing C32: " << C32);
		dd3("Writing C16: " << C16);

		mCrossrefsStream.stream << C32 << C16;
	}
}

void Indexer::printStats()
{
	// Skip if INFO is not enabled

#if INFO
	ii("========================");
	ii("==== INDEXING STATS ====");
	ii("========================");

	ii("Time: " << 	Util::Time::humanTime(INT(mStats.timer.elapsed())));

	// Count

	ii("## Elements: " <<  mCurrentSerial);
	ii("## Terms: " << mIndexTerms.size());
	ii("## Posts: " << mStats.postsCount);
	ii("## Highest field num: " << mStats.highestFieldNumber);
	ii("## Highest in field pos: " << mStats.highestInFieldPosition);
	ii("## Total term ref: " << mStats.termRefCount);
	ii("## Large term ref: " << mStats.largeTermRefCount);
	ii("%% Large term ref: " <<
	   DBLFIX(DOUBLE(mStats.largeTermRefCount) * 100 / mStats.termRefCount, 8)
	   << "%");
	// File sizes

	ii("SZ .idix: " << Util::File::humanSize(mIdentifiersStream.file));
	ii("SZ .plix: " << Util::File::humanSize(mPostingsStream.file));
	ii("SZ .vix: " << Util::File::humanSize(mVocabularyStream.file));
	ii("SZ .cix: " << Util::File::humanSize(mCrossrefsStream.file));
	ii("SZ .xpix: " << Util::File::humanSize(mXmlPositionsStream.file));

	// Other

	QString maxPostsCountTerm = "";
	quint64 maxPostCount = 0;

	for (auto it = mIndexTerms.cbegin(); it != mIndexTerms.cend(); ++it) {
		quint64 postsCount = it.value().stats.postsCount;
		if (postsCount > maxPostCount) {
			maxPostCount = postsCount;
			maxPostsCountTerm = it.key();
		}
	}

	ii("MAX posts count belongs to term '" << maxPostsCountTerm << "' " <<
	   " (" << maxPostCount << ")");

#endif
}
