#include "handler.h"
#include "commons/shared/shared.h"
#include "commons/util/util.h"

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
	dd("Loading keys index file at: " << keysPath);

	mKeysFile.setFileName(keysPath);

	if (!mKeysFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mKeysStream.setDevice(&mKeysFile);

	// Vocabulary file

	QString vocabularyPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::VOCABULARY});
	dd("Loading vocabulary index file at: " << vocabularyPath);

	mVocabularyFile.setFileName(vocabularyPath);

	if (!mVocabularyFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mVocabularyStream.setDevice(&mVocabularyFile);


	// Posting list file

	QString postingListPath = Util::File::path(
		{mIndexPath, mBaseIndexName + Shared::Index::Extensions::POSTING_LIST});
	dd("Loading posting list index file at: " << vocabularyPath);

	mPostingListFile.setFileName(postingListPath);

	if (!mPostingListFile.open(QFile::ReadOnly)) {
		throw	"Cannot read index file. "
				"Is the path valid? Does the index exists with a standard name?";
	}

	mPostingListStream.setDevice(&mPostingListFile);

	// Load runtime stuff

	loadKeys();
	loadVocabulary();

	debug_printKeys();
	debug_printVocabulary();
}

void IndexHandler::debug_findArticlesOfAuthor(const QString &author)
{
	auto refIt = mVocabulary.find(author);

	if (refIt == mVocabulary.end()) {
		ww("Nothing found for author: " << author);
		return;
	}

	IndexTermRef &ref = refIt.value();

	ii("Starting position of " << author << " is " << ref.postingListPosition);
	ii("Term refs to " << ref.article.author);

	qint64 firstArticleAuthorPos = postingListPostPosition(
				ref.postingListPosition, ref.article.author.offset);

	ii("Starting position of first article.author is: " << firstArticleAuthorPos);

	for (int i = 0; i < ref.article.author.count; i++) {
		ii(i << ") article.author of " << ref.article.author.count);
		quint32 P32;
		quint8 P8;

		qint64 postPos = firstArticleAuthorPos +
				static_cast<quint32>(i) * PostingListConf::POST_BYTES;

		mPostingListFile.seek(postPos);

		mPostingListStream >> P32 >> P8;

		int elementId = P32 >> PostingListConf::FIELD_NUM_BITS;
		int fieldNumber = P32 & (~0u >> (32 - PostingListConf::FIELD_NUM_BITS));
		int inFieldPos = P8;
							;
		ii("Post pos = " << postPos);
		ii("P32 b= " << QString::number(P32, 2));
		ii("P8 b= " << QString::number(P8, 2));
		ii("Element id = " << elementId);
		ii("Filed num = " << fieldNumber);
		ii("In field pos = " << inFieldPos);

		ii("Associated key = " << mKeys.at(elementId).key);
		ii("---");
	}

}

void IndexHandler::loadKeys()
{
	dd("Loading keys file index");

	// The format is
	// <posting_list_pos_0>
	// <k0>
	// <k1>
	// ...
	// \n
	// <posting_list_pos_1>
	// <kn>

	bool validFormat;

	qint64 pos = 0;
	qint64 bufferSize = 0;

	QString line, secondLine, thirdLine, key;

	// The first line contains the initial buffer size


	while (!mKeysStream.atEnd()) {
		line = mKeysStream.readLine();

		if (line.isEmpty()) {
			secondLine = mKeysStream.readLine();
			if (secondLine.isEmpty()) {
				// New buffer size (double \n detected)
				thirdLine = mKeysStream.readLine();
				bufferSize = thirdLine.trimmed().toLongLong(&validFormat);
				Q_ASSERT_X(validFormat, "handling", "Unexpected keys index file format");
				vv("Read new keys index buffer size: " << bufferSize);
				continue;
			} else {
				// We have read a key...
				key = secondLine;
			}

			// By the way, increase the current pos
			pos += bufferSize;
		} else {
			// We have read a key...
			key = line;
		}

		mKeys.append(IndexKey {key, pos});
	}
}

void IndexHandler::loadVocabulary()
{
	dd("Loading vocabulary file index");

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

void IndexHandler::debug_printKeys()
{
	dd("Loaded keys are: ");
	foreach(IndexKey k, mKeys) {
		dd("-- " << k.key << " @ " << k.originalFilePosition);
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
