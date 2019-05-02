#include <QHash>
#include <math.h>
#include "index_handler.h"
#include "commons/shared/shared.h"
#include "commons/util/util.h"
#include "commons/const/const.h"

Logger IndexHandler::L = Logger::forClass("IndexHandler");

// --- Hashing purpose

typedef struct ElementField {
	quint32 element;
	quint32 fieldNumber;
} ElementField;


inline bool operator==(const ElementField &ef1, const ElementField &ef2)
{
	return ef1.element == ef2.element &&
			ef1.fieldNumber == ef2.fieldNumber;
}

inline uint qHash(const ElementField &ef, uint seed)
{
	return qHash(ef.element, seed) ^ ef.fieldNumber;
}

inline bool operator==(const IndexPost &ip1, const IndexPost &ip2)
{
	return ip1.elementId == ip2.elementId &&
			ip1.fieldNumber == ip2.fieldNumber &&
			ip1.inFieldTermPosition == ip2.inFieldTermPosition;
}

inline uint qHash(const IndexPost &ip, uint seed)
{
	return qHash(ip.elementId, seed) ^ ip.fieldNumber << ip.inFieldTermPosition;
}

inline bool operator==(const ElementFieldMatch &efm1, const ElementFieldMatch &efm2)
{
	return efm1.elementId == efm2.elementId &&
			efm1.termCount == efm2.termCount;
}

inline uint qHash(const ElementFieldMatch &efm, uint seed)
{
	return qHash(efm.elementId, seed) ^ efm.termCount;
}

// ---

IndexHandler::IndexHandler(QString &indexPath, QString &baseName)
{
	mIndexPath = indexPath;
	mBaseIndexName = baseName;
}

void IndexHandler::load()
{
	// Open files

	// Keys file

	QString keysPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::KEYS});
	ii("Loading keys index file at: " << keysPath);

	mKeysFile.setFileName(keysPath);

	if (!mKeysFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mKeysStream.setDevice(&mKeysFile);

	// Vocabulary file

	QString vocabularyPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::VOCABULARY});
	ii("Loading vocabulary index file at: " << vocabularyPath);

	mVocabularyFile.setFileName(vocabularyPath);

	if (!mVocabularyFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mVocabularyStream.setDevice(&mVocabularyFile);

	// Posting list file

	QString postingListPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::POSTING_LIST});
	ii("Loading posting list index file at: " << postingListPath);

	mPostingListFile.setFileName(postingListPath);

	if (!mPostingListFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mPostingListStream.setDevice(&mPostingListFile);

	// Elements positions (optional)

	QString elementsPosPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::ELEMENTS_POS});
	ii("Loading elements pos index file at: " << elementsPosPath);

	mElementsPosFile.setFileName(elementsPosPath);

	if (!mElementsPosFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mElementsPosStream.setDevice(&mElementsPosFile);

	// Load runtime stuff

	loadKeys();
	loadVocabulary();
	computeIefs();

	debug_printKeys();
	debug_printVocabulary();
	debug_printIefs();
}

bool IndexHandler::findElements(const QString &phrase,
								IndexElementFieldType field,
								QSet<ElementFieldMatch> &matches)
{
	// If the phrase is actually a space separeted list of words, split those
	// and use them as tokens for findElements();

	// Moreover, sanitize each term (lowercase, no punctuation, (trimmed))
	QStringList tokens = phrase.split(
				Const::Dblp::Query::TOKENS_SPLITTER,
				QString::SplitBehavior::SkipEmptyParts);
	for (auto it = tokens.begin(); it < tokens.end(); it++) {
		*it = Util::String::sanitizeTerm(*it);
	}

	return findElements(tokens, field, matches);
}

bool IndexHandler::findElements(const QStringList &tokens,
								IndexElementFieldType field,
								QSet<ElementFieldMatch> &matches) {
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

	ii("Finding elements for " << tokens.join(" "));

	// Maps a field (element+field) to a term-positions
	QHash<
			// elementId + fieldNumber (the field type is implicit and is only one)
			ElementField,
			QHash<
				// Term
				QString,
				// Term positions in element/field
				QSet<quint32> // tf_t
			>
	> categorizedTerms;

	// 1) For each term in the phrase, retrieve the associated posts
	// and push those to an hash that maps <element,field> to <term,positions>

	for (auto it = tokens.begin(); it != tokens.end(); it++) {
		// Retrieve the posts for the this term

		const QString &term = *it;

		QSet<IndexPost> termPosts;
		if (!findPosts(term, field, termPosts))
			continue; // nothing found

		foreach (IndexPost post, termPosts) {
			// Check if this field has already been found
			ElementField ef {post.elementId, post.fieldNumber};

			if (!categorizedTerms.contains(ef)) {
				vv("Inserting ef " <<ef.element << ", "
				   << ef.fieldNumber << " for the first time" );
				// Push a new term to positions map, empty for now
				categorizedTerms.insert(ef, {});
			}

			// Take the termPositions, whether it is the existing one or
			// the just inserted one

			auto termsPosIt = categorizedTerms.find(ef);

			Q_ASSERT_X(termsPosIt != categorizedTerms.end(), "index_handling",
					   "Elements retrieval failed; is this a programming error?");

			QHash<QString, QSet<quint32>> &termsPositions = termsPosIt.value();

			// Check if this term is already present for this element/field

			if (!termsPositions.contains(term)) {
				vv("Inserting " << term << " for the first time for element: "
				   << ef.element << ", field number: " << ef.fieldNumber);
				// Push a new positions list, empty for now
				termsPositions.insert(term, {});
			}

			// Take a reference to the term positions

			auto termPosIt = termsPositions.find(term);

			Q_ASSERT_X(termPosIt != termsPositions.end(), "index_handling",
					   "Elements retrieval failed; is this a programming error?");

			// Push the position of the term in the post field's for this term

			QSet<quint32> &termPositions = termPosIt.value();
			termPositions.insert(post.inFieldTermPosition);
			vv("- pushing pos " << post.inFieldTermPosition << " for term " << term);
		}
	}

	// 2) For each element/field retrieved do the following
	// - for each term of the phrase, do a parallel scan between the terms'
	//   positions lists and check that the position of the terms are sequential
	//   according to the phrase order

	for (auto it = categorizedTerms.begin(); it != categorizedTerms.end(); it++) {
		const ElementField &ef = it.key();
		const QHash<QString, QSet<quint32>> &termsPositions = it.value();

		// Potentially we have a match, but first check for consecutiveness
		// of the phrase terms within the element/field

		// TODO: start from the term with the smallest position list size

		// Take the first positions list (in the future: the smallest one)
		auto ps0It = termsPositions.find(tokens.at(0));

		if (ps0It == termsPositions.end()) {
			// term doesn't existing within this element/field
			continue;  // go asap to the next ef
		}

		QSet<quint32> ps0 = ps0It.value();

		// How many times the phrase (or the word) matches within the element/field
		// This is useful just for figure out the tf and perform ranking over
		// the elements, otherwise it won't be needed since if the elemenet/field
		// contains the phrase it would be enough to present it as a match
		quint8 phrasalMatchesCount = 0;

		foreach (quint32 ps0_pos, ps0) {
			vv("ps0_pos: " << ps0_pos);

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
				vv("ti: " << ti);

				quint32 uti = static_cast<quint32>(ti);

				// Check if the current term exist at the right position
				auto psxIt = termsPositions.find(tokens.at(ti));
				if (psxIt == termsPositions.end()) {
					// term doesn't existing within this element/field
					abort = true; // go asap to the next ef
					break;
				}

				// The term exists, check the position

				QSet<quint32> psx = psxIt.value();

				if (!psx.contains(ps0_pos + uti)) {
					vv("No partial match =(");
					break; // goto next p0_pos
				}
#if 0
				bool partialMatch = false;
				foreach (quint32 psx_pos, psx) {
					vv("psx0_pos: " << ps0_pos);

					// The right position is the one so that
					// psx_pos - p0_pos = ti

					if (psx_pos == ps0_pos + uti) {
						partialMatch = true;
						vv("Partial match!");
						break; // Right position found; no reason to go ahead
					}

					// Furthermore we can quit if we have reach a position
					// greater then the right one, since the position list is ordered
					if (psx_pos > ps0_pos + uti)
						break;
				}

				// In case we have a partial matching it means that until now
				// everything match, so go ahead with the next phrase's term check
				// Otherwise, this p0_pos doesn't give a match, jump to the next
				// p0_pos

				if (!partialMatch) {
					vv("No partial match =(");
					break; // goto next p0_pos
				}
#endif
			}

			if (abort) {
				vv("Aborted, go to next element/field");
				break;
			}

			if (ti == tokens.size()) {
				// We have seen every phrase's term and we are here; it means
				// that all terms's positions were ok for this p0_pos

				// At this point we are sure the ef.element is a match, but we
				// do not push it right now, instead do it later after figuring
				// out how many times the phrase matches within this element/field

				// Just increase the matches count.
				ii("Found a phrasal match!");
				phrasalMatchesCount ++;
			}
		}

		// We can present the element as a match if phrasalMatchesCount
		// is greater than 0

		if (phrasalMatchesCount > 0) {
			// The match count of the phrase is phrasalMatchesCount, but we
			// instead want the number of matches referred to the words, to
			// we have to multiply it by the number of tokens (since all the
			// words should have matched)
			quint8 tf = static_cast<quint8>(phrasalMatchesCount * tokens.size());
			vv("Inserting match with tf: " << tf);
			matches.insert({ef.element, tf});
		}
	}

	ii("# matchings: " << matches.size());

	return true;
}

// NOT SUPPORTED ANYMORE
// Search on more fields at once
//bool IndexHandler::findPosts(const QString &term,
//							 IndexElementFieldTypes fields,
//							 QSet<IndexPost> &posts)
//{
//	ii("Finding posts for " << term << "in fields : " << fields;);

//	bool found = false;
//	// For each possibile field inside fields, test whether the flag
//	// in fields is set; if so extend the search to this field to
//	for (int f = IndexElementFieldTypeStart;
//		 f <= IndexElementFieldTypeEnd;
//		 f = f << 1) {

//		IndexElementFieldType fieldFlag = static_cast<IndexElementFieldType>(f);
//		if (fields.testFlag(fieldFlag))
//			found |= findPosts(term, fieldFlag, posts);
//		// else:  do not search within this field
//	}

//	return found;
//}

bool IndexHandler::findPosts(const QString &term,
							 IndexElementFieldType field,
							 QSet<IndexPost> &posts)
{
	vv("Finding posts for " << term << " in field : " << field;);

	auto refIt = mVocabulary.find(term);

	if (refIt == mVocabulary.end()) {
		// Term not found in the vocabulary
		return false;
	}

	IndexTermRef &ref = refIt.value();

	findPosts(term, ref, field, posts);

	return true;
}

void IndexHandler::findPosts(const QString &term, IndexTermRef &ref,
							 IndexElementFieldType field, QSet<IndexPost> &posts)
{
	// Calculate the field relative position within the ref based on the field
	IndexTermRefPostMeta *refPost;

	switch (field) {
	case ArticleAuthor:
		refPost = &ref.article.author;
		break;
	case ArticleTitle:
		refPost = &ref.article.title;
		break;
	case ArticleYear:
		refPost = &ref.article.year;
		break;

	case IncollectionAuthor:
		refPost = &ref.incollection.author;
		break;
	case IncollectionTitle:
		refPost = &ref.incollection.title;
		break;
	case IncollectionYear:
		refPost = &ref.incollection.year;
		break;
	case IncollectionBooktitle:
		refPost = &ref.incollection.booktitle;
		break;

	case InproceedingsAuthor:
		refPost = &ref.inproceedings.author;
		break;
	case InproceedingsTitle:
		refPost = &ref.inproceedings.title;
		break;
	case InproceedingsYear:
		refPost = &ref.inproceedings.year;
		break;
	case InproceedingsBooktitle:
		refPost = &ref.inproceedings.booktitle;
		break;

	case PhdthesisAuthor:
		refPost = &ref.phdthesis.author;
		break;
	case PhdthesisTitle:
		refPost = &ref.phdthesis.title;
		break;
	case PhdthesisYear:
		refPost = &ref.phdthesis.year;
		break;

	case MasterthesisAuthor:
		refPost = &ref.masterthesis.author;
		break;
	case MasterthesisTitle:
		refPost = &ref.masterthesis.title;
		break;
	case MasterthesisYear:
		refPost = &ref.masterthesis.year;
		break;

	case BookAuthor:
		refPost = &ref.book.author;
		break;
	case BookTitle:
		refPost = &ref.book.title;
		break;
	case BookYear:
		refPost = &ref.book.year;
		break;
	case BookPublisher:
		refPost = &ref.book.publisher;
		break;

	case ProceedingsTitle:
		refPost = &ref.proceedings.title;
		break;
	case ProceedingsYear:
		refPost = &ref.proceedings.year;
		break;
	case ProceedingsPublisher:
		refPost = &ref.proceedings.publisher;
		break;
	case ProceedingsBooktitle:
		refPost = &ref.proceedings.booktitle;
		break;
	default:
		refPost = nullptr;
	}

	Q_ASSERT_X(refPost != nullptr, "index_handling",
			   QString(QString("Unexpected IndexElementFieldType found: " +
					QString::number(field))).toStdString().c_str());

	dd("Found " << refPost->count << " posts for field " << field << " and term '" << term << "'");

	for (quint32 i = 0; i < refPost->count; i++) {
		// Figure out the position of the posts in the posting list (of the first one)
		qint64 postPos = ref.postingListPosition +
				(refPost->offset + i) * PostingListConf::POST_BYTES;

		ii(i << "° post pos: " << postPos);

		// Go the the figured out position
		mPostingListFile.seek(postPos);

		// Read 5 bytes
		quint32 P32;
		quint8 P8;

		mPostingListStream >> P32 >> P8;

		// Figure out element id, field nubmer and field pos
		quint32 elementId = P32 >> PostingListConf::FIELD_NUM_BITS;
		quint32 fieldNumber = P32 & (~0u >> (32 - PostingListConf::FIELD_NUM_BITS));
		quint32 inFieldPos = P8;

		vv("- element id = " << elementId);
		vv("- field num = " << fieldNumber);
		vv("- in field pos = " << inFieldPos);
		vv("- associated key = " << mKeys.at(static_cast<int>(elementId)).key);

		// Push the post
		posts.insert(IndexPost {
			 elementId, fieldNumber,  inFieldPos
		});
#if 0
		qint64 elementPos =
			elementId * Shared::Index::ElementsPosition::ELEMENT_POS_BYTES;

		mElementsPosFile.seek(elementPos);

		quint32 originalFilePos;

		mElementsPosStream >> originalFilePos;

		ii("Original file position = " << originalFilePos);
#endif

	}
}




void IndexHandler::debug_findArticlesOfAuthor(const QString &author)
{
	QSet<IndexPost> posts;
	findPosts(author, ArticleAuthor, posts);
}

void IndexHandler::loadKeys()
{
	dd("Loading keys file index");

	// The format is
	// <k0>
	// <k1>
	// ...
	// <kn>

	// The first line contains the initial buffer size


	while (!mKeysStream.atEnd()) {
		QString key = mKeysStream.readLine();
		mKeys.append({key});
	}
}

void IndexHandler::loadVocabulary()
{
	dd("Loading vocabulary file index");

	// Read the count of posts for each term's field
	// An unit of offset means 5 bytes in the posting list file
	auto loadIndexTermReference =
			[this](IndexTermRefPostMeta &post, quint32 offset) -> quint32 {
		// First of all read 16 bits, than if the first bit is = 1
		// it means that this IndexTermRefPostMeta needs 32 bits for represent
		// the count of posts for this term's field
		quint16 shrinkedCount;
		mVocabularyStream >> shrinkedCount;

		if ((shrinkedCount & VocabularyConf::REF_SHRINKED_FLAG) == 0) {
			// The post meta refers to no more than 2^15 posts per field
			post.count = shrinkedCount;
		} else {
			vv("-- readed a post meta that needs more than 15 bits");

			quint16 countExtension;
			mVocabularyStream >> countExtension;

			post.count =
				// Take the shrinkedCount as first bits and countExtension
				// as lasts, than remove the leftermost bit which is just
				// a marker for denote that more than 15 bits are needed,
				// but actually doesn't count for the int value
				// Not removing it leads to have always a surplus of 2^31
				((static_cast<quint32>(shrinkedCount) << VocabularyConf::REF_SHRINKED_BITS) |
				static_cast<quint32>(countExtension))
					& ~VocabularyConf::REF_EXTENDED_FLAG;
		}

		post.offset = offset;
		return post.count;
	};

	while (!mVocabularyStream.atEnd()) {
		// Read term
		char *term;
		uint len;
		mVocabularyStream.readBytes(term, len);

		vv("-- vocabulary term: " << term);

		// Read starting position of term's refs in posting list

		IndexTermRef ref;
		mVocabularyStream >> ref.postingListPosition;
		quint32 incrementalOffset = 0;

		vv("-- starting position in posting list: " << ref.postingListPosition);

		// <art.a> <art.t> <art.y>
		// <inc.a> <inc.t> <inc.y> <inc.b>
		// <inp.a> <inp.t> <inp.y> <inp.b>
		// <phd.a> <phd.t> <phd.y>
		// <mas.a> <mas.t> <mas.y>
		// <bok.a> <bok.t> <bok.y> <bok.p>
		// <pro.t> <pro.y> <pro.p> <pro.b>

		incrementalOffset += loadIndexTermReference(ref.article.author, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.title, incrementalOffset);
		incrementalOffset += loadIndexTermReference(ref.article.year, incrementalOffset);

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

		vv("-- loaded: " << ref);

		mVocabulary.insert(QString(term), ref);
	}
}

void IndexHandler::computeIefs()
{
	ii("Computing inverse element frequencies...");
	// Compute the inverse element frequency which is defined as
	// ief_t = log10( |E| / ef_t)
	// Whereas ef_t is the element frequnecy, thus the numbers of elements
	// that contains t (must be < |E|)
	// E := number of elements ( = |mKeys| )

	// Thus what we have to do is:
	// Foreach term inside the the vocabulary do:
	// Find the post lists for the term, scan each post and put the element id
	// in set; at the end of the post list, the size of the set contains
	// the number of elements that contain the term.

	for (auto it = mVocabulary.begin(); it != mVocabulary.end(); it++) {
		QString term = it.key();
		IndexTermRef &ref = it.value();

		vv("Computing ief of " << term);

		// We have to invoke loadPosts() for each possible field

		QSet<quint32> elementsWithTerm;

		for (int f = IndexElementFieldTypeStart;
			 f <= IndexElementFieldTypeEnd;
			 f = f << 1) {
			IndexElementFieldType type = static_cast<IndexElementFieldType>(f);

			// Take the posts that contains this term within this field
			QSet<IndexPost> posts;
			findPosts(term, ref, type, posts);

			// At this point posts contains the posts of the term within the field,
			// which may contain duplicate element id if an element.field contains
			// the same term more than once.
			// For this reason, we have to scan the posts and create another
			// set from that avoids elements duplicates

			foreach (IndexPost post, posts) {
				// Just push the element id, we don't care were it actually
				// is present nor the in field position
				elementsWithTerm.insert(post.elementId);
			}
		}

		const double E = mKeys.size();
		const double ef_t = elementsWithTerm.size();

		Q_ASSERT_X(ef_t > 0, "index_handling",
			QString(
				QString("Element frequency must be greater than 0 since the term is in the vocabulary: ") +
				QString(term)
			).toStdString().c_str());

		Q_ASSERT_X(ef_t <= E, "index_handling",
			QString(
				QString("Element frequency must be not greater than the number of elements (") +
				QString::number(ef_t) + " > " + QString::number(E) + ")"
			).toStdString().c_str());

		// At this point elementsWithTerm contains only the elements ids
		// in which the term occurs; the size of it is actually the ef_t
		float ief_t = static_cast<float>(log10(E / ef_t));

		vv("E" << " = " << E);
		vv("ef(" << term << ")" << " = " << ef_t);
		vv("ief(" << term << ")" << " = " << ief_t);

		// Finally put the ief_t inside the hash table
		mIefs.insert(term, ief_t);
	}

	dd("Iefs computing finished");
}

void IndexHandler::debug_printKeys()
{
	dd("Loaded keys are: ");
	foreach(IndexKey k, mKeys) {
		dd("-- " << k.key);
	}
}

void IndexHandler::debug_printVocabulary()
{
	dd("Loaded vocabulary is: ");
	for (auto it = mVocabulary.begin(); it != mVocabulary.end(); it++) {
		dd("-- " << it.key());
		dd("--" << it.value());
//		IndexTermReference ref = it.value();
	}
}

void IndexHandler::debug_printIefs()
{
	const int E = mKeys.size();

	dd("Computed iefs are: " << "( |E| = " << E << " )");

	// For print the iefs sorted, put those inside a map (which automatically
	// order by key)

	QMap<float, QString> sortedIefs;

	for (auto it = mIefs.begin(); it != mIefs.end(); it++) {
		sortedIefs.insertMulti(it.value(), it.key());
	}

	for (auto it = sortedIefs.begin(); it != sortedIefs.end(); it++) {
		const float ief = it.key();
		float ef = static_cast<float>(E / pow(10, ief)); // bonus; recompute ef with the inverse formula
		dd("-- ief(" << it.value() << ") = " << ief << " | ef ~= " << ef);

	}
}


