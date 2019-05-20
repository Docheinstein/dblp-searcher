#include <QMap>
#include <math.h>
#include "index_handler.h"
#include "commons/config/config.h"
#include "commons/util/util.h"
#include "commons/const/const.h"
#include "commons/globals/globals.h"
#include "commons/profiler/profiler.h"
#include <QMutex>

LOGGING(IndexHandler, false);

IndexHandler::IndexHandler(const QString &indexPath, const QString &baseName,
						   bool loadPositions)
{
	mIndexPath = indexPath;
	mBaseIndexName = baseName;
	mLoadPositions = loadPositions;
	init();
}

const QVector<QString> IndexHandler::identifiers() const
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

const QHash<elem_serial, QVector<elem_serial> > IndexHandler::inverseCrossrefs() const
{
	return mInverseCrossrefs;
}

bool IndexHandler::inverseCrossref(elem_serial venueSerial,
								   QVector<elem_serial> &publicationSerials) const
{
	auto inverseCrossrefIt = mInverseCrossrefs.find(venueSerial);
	if (inverseCrossrefIt == mInverseCrossrefs.end())
		return false;
	publicationSerials = inverseCrossrefIt.value();
	return true;
}

const QVector<elem_pos> IndexHandler::positions() const
{
	return mElementsPositions;
}

bool IndexHandler::positionRange(elem_serial serial, QPair<elem_pos, elem_pos> &range) const
{
	int ix = INT(serial);
	if (ix >= mElementsPositions.size())
		return false;

	range.first = 0; // lower bound (default value: start of file)
	range.second = mElementsPositions.at(ix); // upper bound

	// For find the lower bound we have to scan the vector backward in order
	// to find a different position; this is not ideal but is not dramatic
	// since there should be not many items with the same file position
	// Furthermore I've figured out that for some element having a lower
	// bound equals to the end of the previous element is not enough,
	// for this reason, check for two different values

	static const int LB_DIFFERENT_POSITIONS_REQUIRED = 2;
	elem_pos lastDifferentPos = range.second;
	int differentPosCount = 0;

	for (int i = ix - 1; i >= 0 &&
		differentPosCount < LB_DIFFERENT_POSITIONS_REQUIRED; i--) {

		if (mElementsPositions.at(i) != lastDifferentPos) {
			// Found a different position
			differentPosCount++;
			lastDifferentPos = mElementsPositions.at(i);
		}
	}

	if (differentPosCount == LB_DIFFERENT_POSITIONS_REQUIRED)
		range.first = lastDifferentPos;
	// else: pos 0 as fallback

	return true;
}

void IndexHandler::load()
{
	// Load indexes into runtime data structures
	loadIdentifiers();
	loadVocabulary();
	loadCrossrefs();
	loadPositions();

	// Print for debug
	printIdentifiers();
	printVocabulary();
	printCrossrefs();
	printPositions();
}

bool IndexHandler::findMatches(const QString &phrase,
								ElementFieldTypes fieldTypes,
							   QVector<IndexMatch> &matches)
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
								QVector<IndexMatch> &matches) {
	PROF_FUNC_BEGIN4

	bool phrasal = tokens.size() > 1;

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

		if (!phrasal)
			somethingFound |= findWordMatches(sanitizedTokens.at(0), fieldFlag, matches);
		else
			somethingFound |= findPhraseMatches(sanitizedTokens, fieldFlag, matches);
	}

	PROF_FUNC_END

	return somethingFound;
}

bool IndexHandler::findWordMatches(const QString &token,
								   ElementFieldType fieldType,
								   QVector<IndexMatch> &matches) {
	PROF_FUNC_BEGIN5

	// Just retrieve the posts and push to matches

	vv("Finding elements for word '" << token << "'");

	QVector<IndexPost> termPosts;

	if (!findPosts(token, fieldType, termPosts))
		return false; // nothing found for the term

	for (auto it = termPosts.cbegin(); it != termPosts.cend(); it++) {
		// For every post, push it as a match
		const IndexPost &post = *it;
		IndexMatch match;
		match.matchedTokens = QStringList({token});
		match.elementSerial = post.elementSerial;
		match.fieldType = fieldType;
		match.fieldNumber = post.fieldNumber;
		match.matchPosition = post.inFieldTermPosition;
		matches.append(match);
	}

	PROF_FUNC_END

	return true;
}

bool IndexHandler::findPhraseMatches(const QStringList &tokens,
									 ElementFieldType fieldType,
									 QVector<IndexMatch> &matches) {

	PROF_FUNC_BEGIN5

	int matchesSizeBefore = matches.size();

	/*
	 * Retrieves the elements that matches the phrase.
	 * This is done by doing:
	 * 1) For each term in the phrase, retrieve the associated posts
	 * and push those to an hash that maps <element,field> to <term,positions>
	 *
	 * [EVENTUALLY, FOR PHRASALS]
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
				QVector<term_pos> // tf_t
			>
	> categorizedTermsByElementField;

	PROF_BEGIN6(findPhraseMatchesReal)

	// 1) For each term in the phrase, retrieve the associated posts
	// and push those to an hash that maps <element,field> to <term,positions>

	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		// Retrieve the posts for the this term

		const QString &term = *it;

		vv1("Handling term: " << term);

		QVector<IndexPost> termPosts;
		if (!findPosts(term, fieldType, termPosts))
			continue; // nothing found

		foreach (IndexPost post, termPosts) {
			// Check if this field has already been found
			ElementSerial_FieldNumber ef {post.elementSerial, post.fieldNumber};

			if (!categorizedTermsByElementField.contains(ef)) {
				vv2("Inserting ef: " <<
					"{" << ef.elementSerial << ", "<< ef.fieldNumber << "}" <<
					" for the first time" );
				// Push a new term to positions map, empty for now
				categorizedTermsByElementField.insert(ef, {});
			}

			// Take the termPositions, whether it is the existing one or
			// the just inserted one

			auto termsPosIt = categorizedTermsByElementField.find(ef);

			ASSERT(termsPosIt != categorizedTermsByElementField.end(), "index_handling",
					   "Elements retrieval failed");

			QHash<QString, QVector<term_pos>> &termsPositions = termsPosIt.value();

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

			QVector<term_pos> &termPositions = termPosIt.value();
			termPositions.append(post.inFieldTermPosition);
			vv2("Pushing term position: " << post.inFieldTermPosition <<
				" (for term '" << term << "')");
		}
	}

	PROF_END(findPhraseMatchesReal)

	PROF_BEGIN6(findPhraseMatchesParallelCheck)

	// Eventually perform phrasal check if the tokens are > 1, otherwise
	// skip it.

//	if (tokens.size() <= 1) {
//		dd("Skipping phrasal check since only one token has been provided");

//		for (auto it = categorizedTermsByElementField.begin();
//			 it != categorizedTermsByElementField.end();
//			 it++) {

//		}
//		// Let's push this match
//		IndexMatch match;
//		match.matchedTokens = tokens;
//		match.elementSerial = ef.elementSerial;
//		match.fieldType = fieldType;
//		match.fieldNumber = ef.fieldNumber;
//		match.matchPosition = ps0_pos;

//		vv1("Pushing a match: " << match);

//		matches.insert(match);
//	}

	// 2) For each element/field retrieved do the following
	// - for each term of the phrase, do a parallel scan between the terms'
	//   positions lists and check that the position of the terms are sequential
	//   according to the phrase order

	for (auto it = categorizedTermsByElementField.begin();
			it != categorizedTermsByElementField.end(); it++) {

		const ElementSerial_FieldNumber &ef = it.key();
		const QHash<QString, QVector<term_pos>> &termsPositions = it.value();

		// Potentially we have a match, but first check for consecutiveness
		// of the phrase terms within the element/field

		// TODO: start from the term with the smallest position list size

		// Take the first positions list (in the future: the smallest one)
		auto ps0It = termsPositions.find(tokens.at(0));

		if (ps0It == termsPositions.end()) {
			// term doesn't existing within this element/field
			continue;  // go asap to the next ef
		}

		const QVector<term_pos> &ps0 = ps0It.value();

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

				const QVector<term_pos> &psx = psxIt.value();

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

				matches.append(match);
			}
		}
	}

	if (matches.size() > matchesSizeBefore) {
		vv1("Found " << matches.size() << " matches");
	} else {
		dd1("No matches found");
	}

	PROF_END(findPhraseMatchesParallelCheck)

	PROF_FUNC_END

	return true;
}

void IndexHandler::init()
{
	static const char * const INDEX_READ_ERROR =
			"Cannot read index file. "
			"Is the path valid? Does the index files exist with a standard name?";

	// Open files

	// Identifiers file
	QString identifiersPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::IDENTIFIERS);
	ii("Loading identifiers index file at: " << identifiersPath);
	if (!mIdentifiersStream.openRead(identifiersPath))
		QUIT(INDEX_READ_ERROR);

	// Posting list file
	QString postingListPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::POSTING_LIST);
	ii("Opening posting list index file at: " << postingListPath);
	if (!mPostingsStream.openRead(postingListPath))
		QUIT(INDEX_READ_ERROR);

	// Vocabulary file
	QString vocabularyPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::VOCABULARY);
	ii("Opening vocabulary index file at: " << vocabularyPath);
	if (!mVocabularyStream.openRead(vocabularyPath))
		QUIT(INDEX_READ_ERROR);

	// Elements positions file
	QString elementsPosPath = Util::Dblp::Index::indexFilePath(
			mIndexPath, mBaseIndexName, Config::Index::Extensions::ELEMENTS_POS);
	ii("Opening elements pos index file at: " << elementsPosPath);
	if (!mElementsPositionsStream.openRead(elementsPosPath))
		QUIT(INDEX_READ_ERROR);

	// Crossrefs file
	QString crossrefsPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Config::Index::Extensions::CROSSREFS});
	ii("Opening crossrefs index file at: " << crossrefsPath);
	if (!mCrossrefsStream.openRead(crossrefsPath))
		QUIT(INDEX_READ_ERROR);
}

bool IndexHandler::findPosts(const QString &term,
							 ElementFieldType field,
							 QVector<IndexPost> &posts)
{
	dd("Finding posts for " << term << " in field : " <<
	   elementFieldTypeString(field));

	auto refIt = mVocabulary.constFind(term);

	if (refIt == mVocabulary.cend()) {
		ww("Term '" << term << "' not found in vocabulary");
		// Term not found in the vocabulary
		return false;
	}

	findPosts(refIt, field, posts);

	return true;
}

void IndexHandler::findPosts(const QMap<QString, IndexTermRef>::const_iterator vocabularyEntry,
							 ElementFieldType field, QVector<IndexPost> &posts)
{
	PROF_FUNC_BEGIN6

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
//	case ElementFieldType::IncollectionBooktitle:
//		refPost = &ref.incollection.booktitle;
//		break;

	case ElementFieldType::InproceedingsAuthor:
		refPost = &ref.inproceedings.author;
		break;
	case ElementFieldType::InproceedingsTitle:
		refPost = &ref.inproceedings.title;
		break;
	case ElementFieldType::InproceedingsYear:
		refPost = &ref.inproceedings.year;
		break;
//	case ElementFieldType::InproceedingsBooktitle:
//		refPost = &ref.inproceedings.booktitle;
//		break;

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
//	case ElementFieldType::ProceedingsBooktitle:
//		refPost = &ref.proceedings.booktitle;
//		break;
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

#if DEBUG
		const QString &term = vocabularyEntry.key();
		dd1(i << "°; post pos: " << postPos << " of term (" << term << ")");
#endif
		// 5 Bytes that will be read
		quint32 P32;
		quint8 P8;

		// Go the the figured out position and read 5 bytes
		mPostingsStream.file.seek(postPos);
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
		posts.append(IndexPost {
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

	PROF_FUNC_END
}


void IndexHandler::loadIdentifiers()
{
	// The format is
	// <identifier_0>
	// <identifier_1>
	// ...
	// <identifier_n>

	vv("Loading identifiers file index");

	emit identifiersLoadStarted();

	const qint64 identifiersFileSize = mIdentifiersStream.fileSize();

	while (!mIdentifiersStream.stream.atEnd()) {
		QString key = mIdentifiersStream.stream.readLine();
		mIdentifiers.append({key});

		double progress = DOUBLE(mIdentifiersStream.filePosition()) / identifiersFileSize;
		vv1("Identifiers file load progress: " << progress);
		emit identifiersLoadProgress(progress);
	}

	ii("Finished loading of identifiers file (" <<
	   Util::File::humanSize(mIdentifiersStream.file) + ")");

	emit identifiersLoadEnded();
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
		// <inc.a> <inc.t> <inc.y> // <inc.b>
		// <inp.a> <inp.t> <inp.y> // <inp.b>
		// <phd.a> <phd.t> <phd.y>
		// <mas.a> <mas.t> <mas.y>
		// <bok.a> <bok.t> <bok.y> <bok.p>
		// <pro.t> <pro.y> <pro.p> // <pro.b>

		incrementalOffset += loadIndexTermReference(ref.article.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.year, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.journal.name, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.incollection.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.incollection.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.incollection.year, incrementalOffset);
//		incrementalOffset += loadIndexTermReference(ref.incollection.booktitle, incrementalOffset);

		incrementalOffset += loadIndexTermReference(ref.inproceedings.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.inproceedings.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.inproceedings.year, incrementalOffset);
//		incrementalOffset += loadIndexTermReference(ref.inproceedings.booktitle, incrementalOffset);

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
//		incrementalOffset += loadIndexTermReference(ref.proceedings.booktitle, incrementalOffset);

		vv2("Term reference has been loaded into vocabulary: " << ref);

		mVocabulary.insert(QString(term), ref);

		double progress = DOUBLE(mVocabularyStream.filePosition()) / vocabularyFileSize;
		vv1("Vocabulary file load progress: " << progress);
		emit vocabularyLoadProgress(progress);

		i++;
	}

	ii("Finished loading of vocabulary file (" <<
	   Util::File::humanSize(mVocabularyStream.file) + ")");

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

		// Push the direct crossref
		mCrossrefs.insert(pubElementSerial, venueElementSerial);

		// Push the reverse crossref
		auto inverseCrossrefIt = mInverseCrossrefs.find(venueElementSerial);
		if (inverseCrossrefIt == mInverseCrossrefs.end()) {
			// Push for the first time
			mInverseCrossrefs.insert(venueElementSerial, {pubElementSerial});
		} else {
			// Append to the existing vector
			inverseCrossrefIt.value().append(pubElementSerial);
		}

		double progress = DOUBLE(mCrossrefsStream.filePosition()) / crossrefsFileSize;
		vv1("Crossrefs file load progress: " << progress);
		emit crossrefsLoadProgress(progress);
	}

	ii("Finished loading of crossrefs file (" <<
	   Util::File::humanSize(mCrossrefsStream.file) + ")");

	emit crossrefsLoadEnded();
}

void IndexHandler::loadPositions()
{
	if (!mLoadPositions) {
		dd("Not loading positions since not required (XML path not specified)");
		return;
	}
	// The format is
	// <pos_0>
	// <pos_1>
	// ...
	// <pos_0>

	vv("Loading positions file index");

	emit positionsLoadStarted();

	const qint64 posFileSize = mElementsPositionsStream.fileSize();

	while (!mElementsPositionsStream.stream.atEnd()) {
		elem_pos pos;
		mElementsPositionsStream.stream >> pos;
		mElementsPositions.append(pos);

		double progress = DOUBLE(mElementsPositionsStream.filePosition()) / posFileSize;
		vv1("Positions file load progress: " << progress);
		emit positionsLoadProgress(progress);
	}

	ii("Finished loading of positions file (" <<
	   Util::File::humanSize(mElementsPositionsStream.file) + ")");

	emit positionsLoadEnded();
}

void IndexHandler::printIdentifiers()
{
#if VERBOSE
	vv("==== IDENTIFIERS ====");
	elem_serial i = 0;
	foreach(QString id, mIdentifiers) {
		vv1("[" << i << "] : " << id);
		i++;
	}
	vv("==== IDENTIFIERS END ====");
#endif
}

void IndexHandler::printVocabulary()
{
#if VERBOSE
	vv("==== VOCABULARY ====");
	for (auto it = mVocabulary.begin(); it != mVocabulary.end(); it++) {
		vv1("'" << it.key() << "' : " << it.value());
	}
	vv("==== VOCABULARY END ====");
#endif
}

void IndexHandler::printCrossrefs()
{
#if VERBOSE
	vv("==== CROSSREFS ====");
	for (auto it = mCrossrefs.begin(); it != mCrossrefs.end(); it++) {
		vv1(it.key() << " => " << it.value());
	}
	vv("==== CROSSREFS END ====");
#endif
}

void IndexHandler::printPositions()
{
	if (!mLoadPositions)
		return;

#if VERBOSE
	vv("==== POSITIONS ====");
	elem_serial i = 0;
	for (const elem_pos pos : mElementsPositions) {
		vv1("[" << i << "] : " << pos);
		i++;
	}
	vv("==== POSITIONS END ====");
#endif
}

// --- Hashing purpose

bool operator==(const ElementSerial_FieldNumber &ef1,
				const ElementSerial_FieldNumber &ef2)
{
	return ef1.elementSerial == ef2.elementSerial &&
			ef1.fieldNumber == ef2.fieldNumber;
}

uint qHash(const ElementSerial_FieldNumber &ef)
{
	return (ef.elementSerial * 71881) ^ ef.fieldNumber;
}

//inline bool operator==(const IndexMatch &efm1, const IndexMatch &efm2)
//{
//	return
//			efm1.elementSerial == efm2.elementSerial
//			//&&
////			efm1.fieldType == efm2.fieldType &&
////			efm1.fieldNumber == efm2.fieldNumber &&
////			efm1.matchPosition == efm2.matchPosition
//	;
//}

//uint qHash(const IndexMatch &efm, uint seed)
//{
//	return qHash(efm.elementSerial * efm.fieldNumber * efm.matchPosition, seed)
//		^ UINT32(efm.fieldType);
//}

// ---

//IndexMatch::operator QString() const
//{
//	return
//			"{serial = " + DEC(elementSerial) + "; field = " +
//			elementFieldTypeString(fieldType) + "; field_num = " +
//			DEC(fieldNumber) + "; match_pos = " + DEC(matchPosition) +
//			"; (phrase = '" + matchedTokens.join(" ") + "}";
//}
