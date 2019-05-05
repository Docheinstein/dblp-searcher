#include "indexer.h"
#include <QDir>

#include "commons/config/config.h"
#include "commons/util/util.h"
#include "commons/globals/globals.h"

Logger Indexer::L = Logger::forClass("Indexer");

Indexer::Indexer(const QString &outputPath, const QString &baseName)
{
	mOutputPath = outputPath;
	mBaseIndexName = baseName;
}

void Indexer::onStart()
{
	// Open files

	if (!QDir::root().mkpath(mOutputPath)) {
		throw	"Cannot create index file directory. "
				"Is the path valid? Do you have permissions?";
	}

	// Keys file

	QString keysPath = Util::File::path(
		{mOutputPath, mBaseIndexName + Config::Index::Extensions::KEYS});
	dd("Creating keys index file at: " << keysPath);

	mKeysFile.setFileName(keysPath);

	if (!mKeysFile.open(QFile::WriteOnly | QFile::Truncate)) {
		throw	"Cannot create index file. "
				"Is the path valid? Do you have permissions?";
	}

	mKeysStream.setDevice(&mKeysFile);

	// Posting list file

	QString postingListPath = Util::File::path(
		{mOutputPath, mBaseIndexName + Config::Index::Extensions::POSTING_LIST});
	dd("Creating posting list index file at: " << postingListPath);

	mPostingListFile.setFileName(postingListPath);

	if (!mPostingListFile.open(QFile::WriteOnly | QFile::Truncate)) {
		throw	"Cannot create index file. "
				"Is the path valid? Do you have permissions?";
	}

	mPostingListStream.setDevice(&mPostingListFile);

	// Vocabulary file

	QString vocabularyPath = Util::File::path(
		{mOutputPath, mBaseIndexName + Config::Index::Extensions::VOCABULARY});
	dd("Creating vocabulary index file at: " << vocabularyPath);

	mVocabularyFile.setFileName(vocabularyPath);

	if (!mVocabularyFile.open(QFile::WriteOnly | QFile::Truncate)) {
		throw	"Cannot create index file. "
				"Is the path valid? Do you have permissions?";
	}

	mVocabularyStream.setDevice(&mVocabularyFile);

	// Elements pos file

	QString elementsPosPath = Util::File::path(
		{mOutputPath, mBaseIndexName + Config::Index::Extensions::ELEMENTS_POS});
	dd("Creating elements pos index file at: " << elementsPosPath);

	mElementsPosFile.setFileName(elementsPosPath);

	if (!mElementsPosFile.open(QFile::WriteOnly | QFile::Truncate)) {
		throw	"Cannot create index file. "
				"Is the path valid? Do you have permissions?";
	}

	mElementsPosStream.setDevice(&mElementsPosFile);
}

void Indexer::onEnd()
{
	dd("Parse ended");

	debug_printIndexTerms();

	dd("Writing posting list and vocabulary");
	createPostingListAndVocabulary();

	dd("Closing index files");

	mKeysFile.close();
	mPostingListFile.close();
	mVocabularyFile.close();

	debug_printIndexStats();
}

void Indexer::onArticle(DblpArticle &article, qint64 pos)
{
	dd("onArticle: " << article);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.article.author.append(post); },
		article.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.article.title.append(post); },
		article.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.article.year.append(post); },
		article.year
	);
//	addField([](DblpIndexTermEntity *term, DblpIndexTermReference ref)
//		{ term.article.journal.append(post); },
//		article.journal
//	);

	writeElement(article.key, pos);
}


void Indexer::onJournal(DblpJournal &journal, qint64 pos)
{
	dd("onJournal: " << journal.name);
}

void Indexer::onIncollection(DblpIncollection &incollection, qint64 pos)
{
	dd("onIncollection: " << incollection);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.author.append(post); },
		incollection.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.title.append(post); },
		incollection.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.year.append(post); },
		incollection.year
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.incollection.booktitle.append(post); },
		incollection.booktitle
	);

	writeElement(incollection.key, pos);
}

void Indexer::onBook(DblpBook &book, qint64 pos)
{
	dd("onBook: " << book);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.book.author.append(post); },
		book.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.book.title.append(post); },
		book.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.book.year.append(post); },
		book.year
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.book.publisher.append(post); },
		book.publisher
	);

	writeElement(book.key, pos);
}

void Indexer::onInproceedings(DblpInproceedings &inproc, qint64 pos)
{
	dd("onInproceeding: " << inproc);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.author.append(post); },
		inproc.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.title.append(post); },
		inproc.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.year.append(post); },
		inproc.year
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.inproceedings.booktitle.append(post); },
		inproc.booktitle
	);

	writeElement(inproc.key, pos);
}

void Indexer::onProceedings(DblpProceedings &proc, qint64 pos)
{
	dd("onProceeding: " << proc);


	addField([](IndexTerm &term, const IndexPost &post)
		{ term.proceedings.title.append(post); },
		proc.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.proceedings.year.append(post); },
		proc.year
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.proceedings.booktitle.append(post); },
		proc.booktitle
	);

	writeElement(proc.key, pos);
}

void Indexer::onPhdThesis(DblpPhdThesis &phd, qint64 pos)
{
	dd("onPhdThesis: " << phd);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.author.append(post); },
		phd.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.title.append(post); },
		phd.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.phdthesis.year.append(post); },
		phd.year
	);

	writeElement(phd.key, pos);
}

void Indexer::onMasterThesis(DblpMasterThesis &master, qint64 pos)
{
	dd("onMasterThesis: " << master);

	addFields([](IndexTerm &term, const IndexPost &post)
		{ term.masterthesis.author.append(post); },
		master.authors
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.masterthesis.title.append(post); },
		master.title
	);
	addField([](IndexTerm &term, const IndexPost &post)
		{ term.masterthesis.year.append(post); },
		master.year
	);

	writeElement(master.key, pos);
}

void Indexer::writeElement(const QString &key, qint64 pos)
{
	writeKey(key);
	writeElementPosition(pos);
	mCurrent.elementId++;
}

void Indexer::writeKey(const QString &key)
{
	Q_ASSERT_X(!key.isEmpty(),
			 "indexing", "Cannot index an empty key");

	Q_ASSERT_X(mCurrent.elementId < PostingListConf::ELEMENT_ID_THRESHOLD,
			 "indexing", "There are more elements than the index's allowed number");

	vv("Writing key to index: " << key << " for element (" << mCurrent.elementId << ")");

//	if (pos > mCurrent.pos) {
//		vv("New kix buffer pos: " << pos);

//		qint64 posDiff = pos - mCurrent.pos;

//		vv("New kix buffer pos-diff is: " << posDiff);

//		// Add \n in any case for distinguish the next chunk
//		mKeysStream << '\n';

//		if (posDiff > mCurrent.readBufferSize) {
//			// The read buffer size is increased
//			mCurrent.readBufferSize = posDiff;
//			vv("New buffer size detected: " << mCurrent.readBufferSize);
//			// Write another \n, so that the read can distinguish if there is
//			// a number by checking for one or two \n (plus the key's one)
//			mKeysStream << '\n' << mCurrent.readBufferSize << '\n';
//		}

//		mCurrent.pos = pos;
//	}

//	mKeysStream << key
//				<< "_elementId=" << mCurrent.elementId
//				<< "_rbs=" << mCurrent.readBufferSize
//				<< "_pos=" << mCurrent.pos <<
//				'\n';

	mKeysStream << key << '\n';

	vv(" - ");
}

void Indexer::writeElementPosition(qint64 pos) {
	Q_ASSERT_X(pos < Config::Index::ElementsPosition::ELEMENT_POS_THRESHOLD,
			 "indexing", "The original element has a file position above the allowed one");

	quint32 P = static_cast<quint32>(pos);
	mElementsPosStream << P;
}

// Create a posting list that contains just a succession of
// <element><field_number><in_field_term_pos> which will be referred by
// the vocabulary

// The order in which the term's posting list is appended is the following

// article
// - author
// - title
// - year
// (- journal)

// incollection
// - author
// - title
// - year
// - booktitle

// inproceedings
// - author
// - title
// - year
// - booktitle

// phdthesis
// - author
// - title
// - year

// masterthesis
// - author
// - title
// - year

// (journal)

// book
// - author
// - title
// - year
// - publisher

// proceedings
// - title
// - year
// - publisher
// - booktitle

void Indexer::createPostingListAndVocabulary()
{
	for (auto it = mIndexTerms.begin(); it != mIndexTerms.end(); it++) {
		QString term = it.key();
		IndexTerm &termEntity = it.value();

		vv1("Handling term: " << term);

		if (term == "0001") {
			ii("Writing term" << term);
			ii("IndexTerm # a.a: " << termEntity.article.author.size());
			ii("IndexTerm # a.t: " << termEntity.article.title.size());
			ii("IndexTerm # a.y: " << termEntity.article.year.size());

			ii("Term: " << term.toUtf8());
			ii("Plix pos" << mPostingListFile.pos());
			ii("Vix pos" << mVocabularyFile.pos());

		}

//		qint64 postingListStartingPos = mPostingListFile.pos();

		writeVocabularyTermMetas(term);

		// Write the term posts to the posting list,
		// one for each field of the elements

		// <art.a> <art.t> <art.y>
		// <inc.a> <inc.t> <inc.y> <inc.b>
		// <inp.a> <inp.t> <inp.y> <inp.b>
		// <phd.a> <phd.t> <phd.y>
		// <mas.a> <mas.t> <mas.y>
		// <bok.a> <bok.t> <bok.y> <bok.p>
		// <pro.t> <pro.y> <pro.p> <pro.b>

		// article
		// - author
		// - title
		// - year
		// (- journal)

		writeFieldPosts(termEntity.article.author);
		writeFieldPosts(termEntity.article.title);
		writeFieldPosts(termEntity.article.year);

		// incollection
		// - author
		// - title
		// - year
		// - booktitle

		writeFieldPosts(termEntity.incollection.author);
		writeFieldPosts(termEntity.incollection.title);
		writeFieldPosts(termEntity.incollection.year);
		writeFieldPosts(termEntity.incollection.booktitle);

		// inproceedings
		// - author
		// - title
		// - year
		// - booktitle

		writeFieldPosts(termEntity.inproceedings.author);
		writeFieldPosts(termEntity.inproceedings.title);
		writeFieldPosts(termEntity.inproceedings.year);
		writeFieldPosts(termEntity.inproceedings.booktitle);

		// phdthesis
		// - author
		// - title
		// - year

		writeFieldPosts(termEntity.phdthesis.author);
		writeFieldPosts(termEntity.phdthesis.title);
		writeFieldPosts(termEntity.phdthesis.year);

		// masterthesis
		// - author
		// - title
		// - year

		writeFieldPosts(termEntity.masterthesis.author);
		writeFieldPosts(termEntity.masterthesis.title);
		writeFieldPosts(termEntity.masterthesis.year);

		// book
		// - author
		// - title
		// - year
		// - publisher

		writeFieldPosts(termEntity.book.author);
		writeFieldPosts(termEntity.book.title);
		writeFieldPosts(termEntity.book.year);
		writeFieldPosts(termEntity.book.publisher);

		// proceedings
		// - title
		// - year
		// - publisher
		// - booktitle

		writeFieldPosts(termEntity.proceedings.title);
		writeFieldPosts(termEntity.proceedings.year);
		writeFieldPosts(termEntity.proceedings.publisher);
		writeFieldPosts(termEntity.proceedings.booktitle);
	}
}

// Returns the bytes written to the posting list
void Indexer::writeFieldPosts(QList<IndexPost> &posts)
{
	// Write the posts to the posting list and append the posts offset
	// the vocabulary.

	foreach(IndexPost post, posts)
		writePostingListTermPosts(post);

	// Just write the count of the posts

	writeVocabularyTermFieldPostsCount(static_cast<quint32>(posts.size()));
}

inline void Indexer::writeVocabularyTermMetas(QString term)
{
	vv2("Writing metas to vocabulary for term" << term.toUtf8());
	vv3(".vix position: " << mVocabularyFile.pos());
	vv3(".plix position: " << mPostingListFile.pos());

	// First of all, write the term to the vocabulary

	mVocabularyStream << term.toUtf8();

	// Then write the starting position of the term in the posting list

	mVocabularyStream << mPostingListFile.pos();
}

inline void Indexer::writeVocabularyTermFieldPostsCount(quint32 count)
{
	// Just append the count post of the field referred to the starting position
	// in the postining list of the term

	// If the count is below REF_SHRINKED_THRESHOLD (32767 = 2^15 - 1)
	// then write a quint16 with the first bit at 0, otherwise write a full quint32
	// with the first bit at 1 (the loader must be aware of the fact that the full
	// quint32 will have the first bit at 1, which should be removed for take the
	// right number)

	if (count < VocabularyConf::REF_SHRINKED_THRESHOLD) {
		// 16 bit
		vv1("Writing count: " << static_cast<quint16>(count));
		mVocabularyStream << static_cast<quint16>(count);
	}
	else {
		Q_ASSERT_X(mCurrent.elementId < VocabularyConf::REF_EXTENDED_THRESHOLD,
				 "indexing", "There are more posts than the index's allowed number");

		// 32 bit, with leftmost bit = 1
		mVocabularyStream <<
			(count | VocabularyConf::REF_EXTENDED_FLAG);
	}
}


// Append in a dummy way, without knowing the context or whatever, the pattern
// <element><field_number><in_field_term_pos> to the posting list.
// Actually 5 bytes are needed for store the information
// For achieve this, push a 32bit uint and then a 8bit uint

inline void Indexer::writePostingListTermPosts(const IndexPost &post)
{
	vv("Writing post" << mPostingListFile.pos());
	vv("Writing elementId: " << post.elementId);
	vv("Writing fieldNum: " << post.fieldNumber);
	vv("Writing inFieldTermPosition: " << post.inFieldTermPosition);

	// Asserts
	Q_ASSERT_X(post.elementId < PostingListConf::ELEMENT_ID_THRESHOLD,
			 "indexing", "There are more elements than the index's allowed number");

	Q_ASSERT_X(post.fieldNumber < PostingListConf::FIELD_NUM_THRESHOLD,
			 "indexing", "There are more equals fields per element than the index's allowed number");

	Q_ASSERT_X(post.inFieldTermPosition < PostingListConf::IN_FIELD_POS_THRESHOLD,
			 "indexing", "There are more terms per field than the index's allowed number");

	quint32 P32 =  static_cast<quint32>(
			(post.elementId << PostingListConf::FIELD_NUM_BITS) |
			post.fieldNumber);

	quint8 P8 =  static_cast<quint8>(post.inFieldTermPosition);

	vv("Writing P32 to posting list Hex: " << HEX(P32));
	vv("Writing P8 to posting list Hex: " << HEX(P8));

	mPostingListStream << P32 << P8;

	vv("New plix buffer pos: " << mPostingListFile.pos());

	debug_mDebugPostsCount++;

	if (post.inFieldTermPosition > debug_mHighestInFieldPos)
		debug_mHighestInFieldPos = post.inFieldTermPosition;

	if (post.fieldNumber > debug_mHighestFieldNumber)
		debug_mHighestFieldNumber = post.fieldNumber;
}


// For each term inside a field among the fields of the dblp entity we
// have to insert a "post" inside the map (indexed with the term as key)
// that keep tracks of the current element id, the field number (necessary
// for distinguish multiple field (e.g. <author>), and the position
// of the term within the field.

inline void Indexer::addFields(void (*termAdder)(IndexTerm &, const IndexPost &),
							QList<QString> &fieldsContent) {

	int fieldsCount = fieldsContent.size();

	Q_ASSERT_X(static_cast<quint32>(fieldsCount) < PostingListConf::FIELD_NUM_THRESHOLD,
			 "indexing", "Found an element with more fields than the index's allowed number");

	for (int i = 0; i < fieldsCount; i++) {
		QString fieldContent = fieldsContent.at(i);
		vv("- field: " << fieldContent);
		addField(termAdder, fieldContent, static_cast<quint32>(i));
	}
}

inline void Indexer::addField(void (*termAdder)(IndexTerm &, const IndexPost &),
						   const QString &fieldContent, quint32 fieldNumber) {
	QStringList terms = fieldContent.split(" ", QString::SplitBehavior::SkipEmptyParts);

	int termsCount = terms.size();
	Q_ASSERT_X(static_cast<quint32>(termsCount) < PostingListConf::IN_FIELD_POS_THRESHOLD,
			 "indexing", "Found a field with more terms than the index's allowed number");

	for (int i = 0; i < termsCount; i++) {
		QString term = terms.at(i);
		vv("-- term: " << term);
		addTerm(termAdder, term, fieldNumber, static_cast<quint32>(i));
	}
}

inline void Indexer::addTerm(void (*termAdder)(IndexTerm &, const IndexPost &),
						  const QString &term, quint32 fieldNumber, quint32 termPosition) {
	// Sanitize term (lowercase, no punctuation, trim, ...)
	QString indexTerm = Util::String::sanitizeTerm(term);

	// Skip the term if it is empty (e.g. if it was a punctuation)

	if (indexTerm.isEmpty())
		return;

//	Q_ASSERT_X(!indexTerm.isEmpty(), "indexing",
//			   QString(
//				   QString("Term addition failed; empty term. Was (") + term
//				   + ")").toStdString().c_str());

	if (!mIndexTerms.contains(indexTerm)) {
		// Insert a new index term entity, empty for now
		mIndexTerms.insert(indexTerm, {});
	}

	// Whether we have inserted a new index term or the map
	// already contained it, termEntity now contains the
	// inserted term entity

	auto termIt = mIndexTerms.find(indexTerm);

	Q_ASSERT_X(termIt != mIndexTerms.end(), "indexing",
			   "Term addition failed; is this a programming error?");

	IndexTerm &termEntity = *termIt;

	vv("--- pushing " <<
		"(Term = " << indexTerm <<
	   " | Element  = " << mCurrent.elementId <<
	   " | Field  = " << fieldNumber <<
	   " | Pos = " << termPosition << ")"
	);

	IndexPost post;
	post.elementId = mCurrent.elementId;
	post.fieldNumber = fieldNumber;
	post.inFieldTermPosition = termPosition;

	if (fieldNumber > 512) {
		ee("--- VERY HIGH FIELD NUM!! " <<
			"(Term = " << indexTerm <<
		   " | Element  = " << mCurrent.elementId <<
		   " | Field  = " << fieldNumber <<
		   " | Pos = " << termPosition << ")"
		);
		exit(-1);
	}

	termAdder(termEntity, post);
	termEntity.debug_refCount++;
}

void Indexer::debug_printIndexTerms()
{
	dd("Index terms:");
	for (auto it = mIndexTerms.begin(); it != mIndexTerms.end(); ++it) {
		IndexTerm &e = *it;
		dd("\tTerm: " << it.key() << "( & " << &it.key() << ")");

		dd("\t== article.author ==");

		for (int i = 0; i < e.article.author.size(); i++) {
			IndexPost post = e.article.author.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== article.title ==");

		for (int i = 0; i < e.article.title.size(); i++) {
			IndexPost post = e.article.title.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== article.year ==");

		for (int i = 0; i < e.article.year.size(); i++) {
			IndexPost post = e.article.year.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== incollection.author ==");

		for (int i = 0; i < e.incollection.author.size(); i++) {
			IndexPost post = e.incollection.author.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== incollection.title ==");

		for (int i = 0; i < e.incollection.title.size(); i++) {
			IndexPost post = e.incollection.title.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== incollection.year ==");

		for (int i = 0; i < e.incollection.year.size(); i++) {
			IndexPost post = e.incollection.year.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== incollection.booktitle ==");

		for (int i = 0; i < e.incollection.booktitle.size(); i++) {
			IndexPost post = e.incollection.booktitle.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== book.author ==");

		for (int i = 0; i < e.book.author.size(); i++) {
			IndexPost post = e.book.author.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== book.title ==");

		for (int i = 0; i < e.book.title.size(); i++) {
			IndexPost post = e.book.title.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

		dd("\t== book.year ==");

		for (int i = 0; i < e.book.year.size(); i++) {
			IndexPost post = e.book.year.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}
		dd("\t== book.publisher ==");

		for (int i = 0; i < e.book.publisher.size(); i++) {
			IndexPost post = e.book.publisher.at(i);
			dd("\t\t" <<
			   "(Element = " << post.elementId <<
			   " | Field = " << post.fieldNumber <<
			   " | Position = " << post.inFieldTermPosition);
		}

	}
}

void Indexer::debug_printIndexStats()
{
	// Count

	ii("## Elements: " <<  mCurrent.elementId);
	ii("## Terms: " << mIndexTerms.size());
	ii("## Posts: " << debug_mDebugPostsCount);
	ii("## Highest field num: " << debug_mHighestFieldNumber);
	ii("## Highest in field pos: " << debug_mHighestInFieldPos);

	// File sizes

	ii("SZ .kix: " << Util::File::humanSize(mKeysFile));
	ii("SZ .plix: " << Util::File::humanSize(mPostingListFile));
	ii("SZ .vix: " << Util::File::humanSize(mVocabularyFile));
	ii("SZ .epix: " << Util::File::humanSize(mElementsPosFile));

	// Other

	QString maxPostsCountStr = "";
	int maxPostCount = 0;

	for (auto it = mIndexTerms.begin(); it != mIndexTerms.end(); it++) {
		int refCount = it.value().debug_refCount;
		if (refCount > maxPostCount) {
			maxPostCount = refCount;
			maxPostsCountStr = it.key();
		}
	}

	ii("MAX refs: " << maxPostsCountStr << " (" << maxPostCount << ")");
}
