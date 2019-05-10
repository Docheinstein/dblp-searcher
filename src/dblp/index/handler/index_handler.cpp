#include <QMap>
#include <math.h>
#include "index_handler.h"
#include "commons/config/config.h"
#include "commons/util/util.h"
#include "commons/const/const.h"
#include "commons/globals/globals.h"

LOGGING(IndexHandler, true);

IndexHandler::IndexHandler(const QString &indexPath, const QString &baseName)
{
	mIndexPath = indexPath;
	mBaseIndexName = baseName;
	init();
}

const QList<QString> IndexHandler::identifiers() const
{
	return mIdentifiers;
}

bool IndexHandler::identifier(elem_serial serial, QString &identifier) const
{
	int ix = INT(serial);
	if (ix >= mIdentifiers.size())
		return false;
	identifier = mIdentifiers.at(ix);
	return true;
}

const QMap<QString, IndexTermRef> IndexHandler::vocabulary() const
{
	return mVocabulary;
}

bool IndexHandler::vocabularyTermRef(const QString &term, IndexTermRef &termRef) const
{
	auto termRefIt = mVocabulary.find(term);
	if (termRefIt == mVocabulary.end())
		return false;
	termRef = termRefIt.value();
	return true;
}



const QHash<elem_serial, elem_serial> IndexHandler::crossrefs() const
{
	return mCrossrefs;
}

bool IndexHandler::crossref(elem_serial publicationSerial, elem_serial &venueSerial) const
{
	auto crossrefIt = mCrossrefs.find(publicationSerial);
	if (crossrefIt == mCrossrefs.end())
		return false;
	venueSerial = crossrefIt.value();
	return true;
}

void IndexHandler::load()
{
	// Load indexes into runtime data structures
	loadIdentifiers();
	loadVocabulary();
	loadCrossrefs();

	// Print for debug
	printIdentifiers();
	printVocabulary();
	printCrossrefs();
}

bool IndexHandler::findMatches(const QString &phrase,
								ElementFieldTypes fieldTypes,
								QSet<IndexMatch> &matches)
{

	// If the phrase is actually a space separeted list of words,
	// split those and use them as tokens for findElements();
	QStringList tokens = phrase.split(
				Const::Dblp::Query::TOKENS_SPLITTER,
				QString::SplitBehavior::SkipEmptyParts);

	return findMatches(tokens, fieldTypes, matches);
}

typedef struct ElementSerial_FieldNumber {
	elem_serial elementSerial;
	field_num fieldNumber;
} ElementSerial_FieldNumber;

bool IndexHandler::findMatches(const QStringList &tokens,
								ElementFieldTypes fieldTypes,
								QSet<IndexMatch> &matches) {
	QStringList sanitizedTokens;

	// Moreover, sanitize each term (lowercase, no punctuation, (trimmed))
	for (auto it = tokens.begin(); it < tokens.end(); it++) {
		sanitizedTokens.append(Util::String::sanitizeTerm(*it));
	}

	bool somethingFound = false;

	// For each possibile field inside fields, test whether the flag
	// in fields is set; if so extend the search to this field to
	for (int f = INT(ElementFieldType::_Start);
		 f <= INT(ElementFieldType::_End);
		 f = f << 1) {

		ElementFieldType fieldFlag = static_cast<ElementFieldType>(f);
		if (!fieldTypes.testFlag(fieldFlag))
			continue; // do not search within this field

		somethingFound |= findMatchesSingleType(tokens, fieldFlag, matches);
	}

	return somethingFound;

}


bool IndexHandler::findMatchesSingleType(const QStringList &tokens,
										  ElementFieldType fieldType,
										  QSet<IndexMatch> &matches)
{

	int matchesSizeBefore = matches.size();

	/*
	 * Retrieves the elements that matches the phrase.
	 * This is done by doing:
	 * 1) For each term in the phrase, retrieve the associated posts
	 * and push those to an hash that maps <element,field> to <term,positions>
	 * 2) For each element/field retrieved do the following
	 * - for each term of the phrase, do a parallel scan between the terms'
	 *   positions lists and check that the position of the terms are sequential
	 *   according to the phrase order
	 */

	vv("Finding elements for '" << tokens.join(" ") <<
	   "' in type " << elementFieldTypeString(fieldType));

	// Maps a field (element+field) to a term-positions
	QHash<
			// elementSerial + fieldNumber (the field type is implicit and is only one)
			ElementSerial_FieldNumber,
			QHash<
				// Term
				QString,
				// Term positions in element/field
				QSet<term_pos> // tf_t
			>
	> categorizedTerms;

	// 1) For each term in the phrase, retrieve the associated posts
	// and push those to an hash that maps <element,field> to <term,positions>

	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		// Retrieve the posts for the this term

		const QString &term = *it;

		vv1("Handling term: " << term);

		QSet<IndexPost> termPosts;
		if (!findPosts(term, fieldType, termPosts))
			continue; // nothing found

		foreach (IndexPost post, termPosts) {
			// Check if this field has already been found
			ElementSerial_FieldNumber ef {post.elementSerial, post.fieldNumber};

			if (!categorizedTerms.contains(ef)) {
				vv2("Inserting ef: " <<
					"{" << ef.elementSerial << ", "<< ef.fieldNumber << "}" <<
					" for the first time" );
				// Push a new term to positions map, empty for now
				categorizedTerms.insert(ef, {});
			}

			// Take the termPositions, whether it is the existing one or
			// the just inserted one

			auto termsPosIt = categorizedTerms.find(ef);

			ASSERT(termsPosIt != categorizedTerms.end(), "index_handling",
					   "Elements retrieval failed");

			QHash<QString, QSet<term_pos>> &termsPositions = termsPosIt.value();

			// Check if this term is already present for this element/field

			if (!termsPositions.contains(term)) {
				vv2("Inserting term '" << term << "' for the first time for ef: "
					"{" << ef.elementSerial << ", "<< ef.fieldNumber << "}");

				// Push a new positions list, empty for now
				termsPositions.insert(term, {});
			}

			// Take a reference to the term positions

			auto termPosIt = termsPositions.find(term);

			ASSERT(termPosIt != termsPositions.end(), "index_handling",
					   "Elements retrieval failed");

			// Push the position of the term in the post field's for this term

			QSet<term_pos> &termPositions = termPosIt.value();
			termPositions.insert(post.inFieldTermPosition);
			vv2("Pushing term position: " << post.inFieldTermPosition <<
				" (for term '" << term << "')");
		}
	}

	// 2) For each element/field retrieved do the following
	// - for each term of the phrase, do a parallel scan between the terms'
	//   positions lists and check that the position of the terms are sequential
	//   according to the phrase order

	for (auto it = categorizedTerms.begin(); it != categorizedTerms.end(); it++) {
		const ElementSerial_FieldNumber &ef = it.key();
		const QHash<QString, QSet<term_pos>> &termsPositions = it.value();

		// Potentially we have a match, but first check for consecutiveness
		// of the phrase terms within the element/field

		// TODO: start from the term with the smallest position list size

		// Take the first positions list (in the future: the smallest one)
		auto ps0It = termsPositions.find(tokens.at(0));

		if (ps0It == termsPositions.end()) {
			// term doesn't existing within this element/field
			continue;  // go asap to the next ef
		}

		QSet<term_pos> ps0 = ps0It.value();

		// How many times the phrase (or the word) matches within the element/field
		// This is useful just for figure out the tf and perform ranking over
		// the elements, otherwise it won't be needed since if the elemenet/field
		// contains the phrase it would be enough to present it as a match
//		quint8 phrasalMatchesCount = 0;

		foreach (term_pos ps0_pos, ps0) {
			dd1("ps0_pos: " << ps0_pos);

			// ps0: positions of the first term
			// ps0_pos: a position of ps0
			// psx: positions of a phrase term != ps0
			// psx_pos: a position of psx
			// ti: term index := index of the term within the phrase

			int ti;

			bool abort = false;

			// Avoid the position list for the term for which the position list
			// has already been taken into ps0
			for (ti = 1; ti < tokens.size(); ti++) {
				dd2("ti: " << ti);

				term_pos uti = UINT8(ti);

				// Check if the current term exist at the right position
				auto psxIt = termsPositions.find(tokens.at(ti));
				if (psxIt == termsPositions.end()) {
					// term doesn't existing within this element/field
					abort = true; // go asap to the next ef
					break;
				}

				// The term exists, check the position

				QSet<term_pos> psx = psxIt.value();

				if (!psx.contains(ps0_pos + uti)) {
					dd2("No partial match =(");
					break; // goto next p0_pos
				}
			}

			if (abort) {
				dd1("Aborted, go to next element/field");
				break;
			}

			if (ti == tokens.size()) {
				// We have seen every phrase's term and we are here; it means
				// that all terms's positions were ok for this p0_pos

				// Let's push this match
				IndexMatch match;
				match.matchedTokens = tokens;
				match.elementSerial = ef.elementSerial;
				match.fieldType = fieldType;
				match.fieldNumber = ef.fieldNumber;
				match.matchPosition = ps0_pos;

				vv1("Pushing a match: " << match);

				matches.insert(match);
			}
		}
	}

	if (matches.size() > matchesSizeBefore) {
		vv1("Found " << matches.size() << " matches");
	} else {
#if !VERBOSE
		dd1("No matches found");
#endif
	}

	return true;
}

void IndexHandler::init()
{

	static const char * const INDEX_READ_ERROR =
			"Cannot read index file. "
			"Is the path valid? Does the index exists with a standard name?";

	// Open files

	// Identifiers file
	QString identifiersPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::IDENTIFIERS);
	ii("Loading identifiers index file at: " << identifiersPath);
	if (!mIdentifiersStream.openRead(identifiersPath))
		throw INDEX_READ_ERROR;

	// Posting list file
	QString postingListPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::POSTING_LIST);
	ii("Opening posting list index file at: " << postingListPath);
	if (!mPostingsStream.openRead(postingListPath))
		throw INDEX_READ_ERROR;

	// Vocabulary file
	QString vocabularyPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::VOCABULARY);
	ii("Opening vocabulary index file at: " << vocabularyPath);
	if (!mVocabularyStream.openRead(vocabularyPath))
		throw INDEX_READ_ERROR;

	// Elements positions file
	QString elementsPosPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::ELEMENTS_POS);
	ii("Opening elements pos index file at: " << elementsPosPath);
	if (!mElementsPositionsStream.openRead(elementsPosPath))
		throw INDEX_READ_ERROR;

	// Crossrefs file
	QString crossrefsPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Config::Index::Extensions::CROSSREFS});
	ii("Opening crossrefs index file at: " << crossrefsPath);
	if (!mCrossrefsStream.openRead(crossrefsPath))
		throw INDEX_READ_ERROR;

	ii("Started parsing of XML file...");
}

bool IndexHandler::findPosts(const QString &term,
							 ElementFieldType field,
							 QSet<IndexPost> &posts)
{
	dd("Finding posts for " << term << " in field : " <<
	   elementFieldTypeString(field));

	auto refIt = mVocabulary.find(term);

	if (refIt == mVocabulary.end()) {
		ww("Term '" << term << "' not found in vocabulary");
		// Term not found in the vocabulary
		return false;
	}

	findPosts(refIt, field, posts);

	return true;
}

void IndexHandler::findPosts(const QMap<QString, IndexTermRef>::const_iterator vocabularyEntry,
							 ElementFieldType field, QSet<IndexPost> &posts)
{
	const QString &term = vocabularyEntry.key();
	const IndexTermRef &ref = vocabularyEntry.value();

	// Calculate the field relative position within the ref based on the field
	const IndexTermRefPostMeta *refPost;

	switch (field) {
	case ElementFieldType::ArticleAuthor:
		refPost = &ref.article.author;
		break;
	case ElementFieldType::ArticleTitle:
		refPost = &ref.article.title;
		break;
	case ElementFieldType::ArticleYear:
		refPost = &ref.article.year;
		break;

	case ElementFieldType::Journal:
		refPost = &ref.journal.name;
		break;

	case ElementFieldType::IncollectionAuthor:
		refPost = &ref.incollection.author;
		break;
	case ElementFieldType::IncollectionTitle:
		refPost = &ref.incollection.title;
		break;
	case ElementFieldType::IncollectionYear:
		refPost = &ref.incollection.year;
		break;
	case ElementFieldType::IncollectionBooktitle:
		refPost = &ref.incollection.booktitle;
		break;

	case ElementFieldType::InproceedingsAuthor:
		refPost = &ref.inproceedings.author;
		break;
	case ElementFieldType::InproceedingsTitle:
		refPost = &ref.inproceedings.title;
		break;
	case ElementFieldType::InproceedingsYear:
		refPost = &ref.inproceedings.year;
		break;
	case ElementFieldType::InproceedingsBooktitle:
		refPost = &ref.inproceedings.booktitle;
		break;

	case ElementFieldType::PhdthesisAuthor:
		refPost = &ref.phdthesis.author;
		break;
	case ElementFieldType::PhdthesisTitle:
		refPost = &ref.phdthesis.title;
		break;
	case ElementFieldType::PhdthesisYear:
		refPost = &ref.phdthesis.year;
		break;

	case ElementFieldType::MasterthesisAuthor:
		refPost = &ref.masterthesis.author;
		break;
	case ElementFieldType::MasterthesisTitle:
		refPost = &ref.masterthesis.title;
		break;
	case ElementFieldType::MasterthesisYear:
		refPost = &ref.masterthesis.year;
		break;

	case ElementFieldType::BookAuthor:
		refPost = &ref.book.author;
		break;
	case ElementFieldType::BookTitle:
		refPost = &ref.book.title;
		break;
	case ElementFieldType::BookYear:
		refPost = &ref.book.year;
		break;
	case ElementFieldType::BookPublisher:
		refPost = &ref.book.publisher;
		break;

	case ElementFieldType::ProceedingsTitle:
		refPost = &ref.proceedings.title;
		break;
	case ElementFieldType::ProceedingsYear:
		refPost = &ref.proceedings.year;
		break;
	case ElementFieldType::ProceedingsPublisher:
		refPost = &ref.proceedings.publisher;
		break;
	case ElementFieldType::ProceedingsBooktitle:
		refPost = &ref.proceedings.booktitle;
		break;
	default:
		refPost = nullptr;
	}

	ASSERT(refPost != nullptr, "index_handling",
		"Unexpected ElementFieldType found: ", elementFieldTypeString(field));

	if (refPost->count > 0) {
		vv("Found " << refPost->count << " posts for field "
		   << elementFieldTypeString(field)
			<< " and term '" << vocabularyEntry.key() << "'");
	} else {
		// Print as debug anyway, if verbose is disabled
		dd("Found " << refPost->count << " posts for field "
		   << elementFieldTypeString(field)
			<< " and term '" << vocabularyEntry.key() << "'");
	}


	for (quint32 i = 0; i < refPost->count; i++) {
		// Figure out the position of the posts in the posting list (of the first one)
		qint64 postPos = ref.postingListPosition +
				(refPost->offset + i) * PostingListConf::POST_BYTES;

		dd1(i << "° post pos: " << postPos << " of term (" << term << ")");

		// Go the the figured out position
		mPostingsStream.file.seek(postPos);

		// Read 5 bytes
		quint32 P32;
		quint8 P8;

		mPostingsStream.stream >> P32 >> P8;

		// Figure out element id, field nubmer and field pos
		quint32 elementSerial = P32 >> PostingListConf::FIELD_NUM_BITS;
		field_num fieldNumber = P32 & (~0u >> (32 - PostingListConf::FIELD_NUM_BITS));
		term_pos inFieldPos = P8;

		dd2("Element serial		= " << elementSerial);
		dd2("Field number		= " << fieldNumber);
		dd2("In field position	= " << inFieldPos);
		dd2("(Identifier)		= " << mIdentifiers.at(INT(elementSerial)));

		// Push the post
		posts.insert(IndexPost {
			 elementSerial, fieldNumber,  inFieldPos
		});

		// Position...
#if 0
		qint64 elementPos =
			elementSerial * Shared::Index::ElementsPosition::ELEMENT_POS_BYTES;

		mElementsPosFile.seek(elementPos);

		quint32 originalFilePos;

		mElementsPosStream >> originalFilePos;

		ii("Original file position = " << originalFilePos);
#endif

	}
}


void IndexHandler::loadIdentifiers()
{
	// The format is
	// <identifier_0>
	// <identifier_1>
	// ...
	// <identifier_n>

	vv("Loading identifiers file index");

	emit keysLoadStarted();

	const qint64 keysFileSize = mIdentifiersStream.fileSize();

	while (!mIdentifiersStream.stream.atEnd()) {
		QString key = mIdentifiersStream.stream.readLine();
		mIdentifiers.append({key});

		double progress = DOUBLE(mIdentifiersStream.filePosition()) / keysFileSize;
		vv1("Keys file load progress: " << progress);
		emit keysLoadProgress(progress);
	}

	emit keysLoadEnded();
}


void IndexHandler::loadVocabulary()
{
	vv("Loading vocabulary file index");

	emit vocabularyLoadStarted();

	const qint64 vocabularyFileSize = mVocabularyStream.fileSize();

	// Read the count of posts for each term's field
	// An unit of offset means 5 bytes in the posting list file
	auto loadIndexTermReference =
			[this](IndexTermRefPostMeta &post, quint32 offset) -> quint32 {
		// First of all read 16 bits, than if the first bit is = 1
		// it means that this IndexTermRefPostMeta needs 32 bits for represent
		// the count of posts for this term's field
		quint16 shrinkedCount;
		mVocabularyStream.stream >> shrinkedCount;

		dd("Readed posts count from stream: " << shrinkedCount);

		if ((shrinkedCount & VocabularyConf::REF_SHRINKED_FLAG) == 0) {
			// The post meta refers to no more than 2^15 posts per field
			post.count = shrinkedCount;
		} else {
			vv4("Readed a post meta that needs more than 15 bits");

			quint16 countExtension;
			mVocabularyStream.stream >> countExtension;

			post.count =
				// Take the shrinkedCount as first bits and countExtension
				// as lasts, than remove the leftermost bit which is just
				// a marker for denote that more than 15 bits are needed,
				// but actually doesn't count for the int value
				// Not removing it leads to have always a surplus of 2^31
				(
				UINT32(shrinkedCount) << (VocabularyConf::REF_SHRINKED_BITS + 1u)
					|
				UINT32(countExtension)
				)
					& ~VocabularyConf::REF_EXTENDED_FLAG;
		}

		post.offset = offset;
		return post.count;
	};

	int i = 0;

	while (!mVocabularyStream.stream.atEnd()) {
		// Read term
		char *term;
		uint len;
		mVocabularyStream.stream.readBytes(term, len);

		if (!term) {
			ww("Term load failure at vocabulary position: " <<
			   mVocabularyStream.filePosition());
			continue;
		}

		vv1("Loading vocabulary term: " << term);

		// Read starting position of term's refs in posting list

		IndexTermRef ref;
		mVocabularyStream.stream >> ref.postingListPosition;
		quint32 incrementalOffset = 0;


		vv2("Starting position in posting list: " << ref.postingListPosition);

		// <art.a> <art.t> <art.y>
		// <jou>
		// <inc.a> <inc.t> <inc.y> <inc.b>
		// <inp.a> <inp.t> <inp.y> <inp.b>
		// <phd.a> <phd.t> <phd.y>
		// <mas.a> <mas.t> <mas.y>
		// <bok.a> <bok.t> <bok.y> <bok.p>
		// <pro.t> <pro.y> <pro.p> <pro.b>

		incrementalOffset += loadIndexTermReference(ref.article.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.year, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.journal.name, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.incollection.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.incollection.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.incollection.year, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.incollection.booktitle, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.inproceedings.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.inproceedings.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.inproceedings.year, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.inproceedings.booktitle, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.phdthesis.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.phdthesis.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.phdthesis.year, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.masterthesis.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.masterthesis.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.masterthesis.year, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.book.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.book.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.book.year, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.book.publisher, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.proceedings.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.proceedings.year, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.proceedings.publisher, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.proceedings.booktitle, incrementalOffset);

		vv2("Term reference has been loaded into vocabulary: " << ref);

		mVocabulary.insert(QString(term), ref);

		double progress = DOUBLE(mVocabularyStream.filePosition()) / vocabularyFileSize;
		vv1("Vocabulary file load progress: " << progress);
		emit vocabularyLoadProgress(progress);

		i++;
	}

	emit vocabularyLoadEnded();
}

void IndexHandler::loadCrossrefs()
{
	vv("Loading crossrefs file index");

	emit crossrefsLoadStarted();

	const qint64 crossrefsFileSize = mCrossrefsStream.fileSize();

	quint32 pubElementSerial;
	quint32 venueElementSerial;

	while (!mCrossrefsStream.stream.atEnd()) {
		mCrossrefsStream.stream >> pubElementSerial >> venueElementSerial;

		vv1("Loaded crossref " << pubElementSerial << " => " << venueElementSerial);

		mCrossrefs.insert(pubElementSerial, venueElementSerial);

		double progress = DOUBLE(mCrossrefsStream.filePosition()) / crossrefsFileSize;
		vv1("Crossrefs file load progress: " << progress);
		emit crossrefsLoadProgress(progress);
	}

	emit crossrefsLoadEnded();
}

void IndexHandler::printIdentifiers()
{
	vv("==== IDENTIFIERS ====");
	elem_serial i = 0;
	foreach(QString id, mIdentifiers) {
		vv1("[" << i << "] : " << id);
		i++;
	}
	vv("==== IDENTIFIERS END ====");
}

void IndexHandler::printVocabulary()
{
	vv("==== VOCABULARY ====");
	for (auto it = mVocabulary.begin(); it != mVocabulary.end(); it++) {
		vv1("'" << it.key() << "' : " << it.value());
	}
	vv("==== VOCABULARY END ====");
}

void IndexHandler::printCrossrefs()
{
	vv("==== CROSSREFS ====");
	for (auto it = mCrossrefs.begin(); it != mCrossrefs.end(); it++) {
		vv1(it.key() << " => " << it.value());
	}
	vv("==== CROSSREFS END ====");
}

// --- Hashing purpose

bool operator==(const ElementSerial_FieldNumber &ef1,
				const ElementSerial_FieldNumber &ef2)
{
	return ef1.elementSerial == ef2.elementSerial &&
			ef1.fieldNumber == ef2.fieldNumber;
}

uint qHash(const ElementSerial_FieldNumber &ef, uint seed)
{
	return qHash(ef.elementSerial, seed) ^ ef.fieldNumber;
}

bool operator==(const IndexMatch &efm1, const IndexMatch &efm2)
{
	return	efm1.elementSerial == efm2.elementSerial &&
			efm1.fieldType == efm2.fieldType &&
			efm1.fieldNumber == efm2.fieldNumber &&
			efm1.matchPosition == efm2.matchPosition
	;
}

uint qHash(const IndexMatch &efm, uint seed)
{
	return qHash(efm.elementSerial * efm.fieldNumber * efm.matchPosition, seed)
			^ UINT32(efm.fieldType);
}

// ---

IndexMatch::operator QString()
{
	return
			"{serial = " + DEC(elementSerial) + "; field = " +
			elementFieldTypeString(fieldType) + "; field_num = " +
			DEC(fieldNumber) + "; match_pos = " + DEC(matchPosition) +
			"; (phrase = '" + matchedTokens.join(" ") + "}";
}