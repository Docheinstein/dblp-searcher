#include "ir_model_ief.h"

#include <math.h>

#include "commons/globals/globals.h"
#include "commons/config/app_config.h"
#include "dblp/index/models/reference/index_term_ref.h"
#include "dblp/index/handler/index_handler.h"

static const float TERM_NOT_FOUND = -1;

LOGGING(IRModelIef, true)

IRModelIef::~IRModelIef() {}
IRModelIef::IRModelIef(IndexHandler &indexHandler) : IRModel(indexHandler) {}

void IRModelIef::init()
{
	// Calling computeIefs() now will speed up any further call to termScore()
	// but may require some time, for this reason the computing is parametrized
	// and can be done either now or at runtime

#if LAZY_IEF
	dd("Not computing iefs now since lazy initialization has been required");

	// Emit signals any way
	emit initStarted();
	emit initEnded();
#else
	// Compute all the iefs now (may require some time)
	computeIefs();
	printIefs();
#endif
}

float IRModelIef::termScore(const QString &term)
{
	// The term score is actually the tf.ief
	float ief = mIefs.value(term, 0);

	// Ief has already been computed for an existing term, return it
	if (ief > 0)
		return ief;

	// Ief has already been computed but the term doesn't exists, avoid
	// to recompute it
	if (ief < 0)
		return 0;

	// Ief has never been computed for the term, compute it,
	// put in the map and return it
	ief = computeIef(term);

	mIefs.insert(term, ief);

	return ief;
}

float IRModelIef::bonusFactorPerPhraseTerm() const
{
	// Seems reasonable
	return 1.15f;
}

float IRModelIef::bonusFactorForPublicationMatch() const
{
	return 1;
}

float IRModelIef::bonusFactorForVenueMatch() const
{
	// Consider the venues more important?
	return 1.1f;
}

float IRModelIef::bonusFactorForPublicationVenueMatch() const
{
	// For sure a pub+venue is more important..
	return 1.3f;
}

void IRModelIef::computeIefs()
{
	ii("Computing inverse element frequencies...");

	emit initStarted();

	// Compute the inverse element frequency which is defined as
	// ief_t = log10( |E| / ef_t)
	// Whereas ef_t is the element frequency, thus the numbers of elements
	// that contains t (must be < |E|)
	// E := number of elements ( = |mKeys| )

	// Thus what we have to do is:
	// Foreach term inside the the vocabulary do:
	// Find the post lists for the term, scan each post and put the element id
	// in set; at the end of the post list, the size of the set contains
	// the number of elements that contain the term.

	const VOCABULARY_ADT<QString, IndexTermRef> &vocabulary = mIndex.vocabulary();

	const qint64 vocabularySize = vocabulary.size();

	int i = 0;
	for (auto it = vocabulary.cbegin(); it != vocabulary.cend(); ++it, ++i) {
		const QString &term = it.key();

		if (term.isEmpty())
			continue;

		vv1("Computing ief of '" << term << "'");

		float ief = computeIef(it);

		// Finally put the ief_t inside the hash table
		mIefs.insert(term, ief);

		// Notify the current progress
		double progress = DOUBLE(i) / vocabularySize;
		vv1("IRModel Progress: " << progress);

		emit initProgress(progress);
	}

	ii("Iefs computing finished");

	emit initEnded();
}

float IRModelIef::computeIef(const QString &term)
{
	auto termRefIt = mIndex.vocabulary().constFind(term);

	// Term not present in the vocabulary
	if (termRefIt == mIndex.vocabulary().cend())
		return TERM_NOT_FOUND;

	// Really compute the ief
	return computeIef(termRefIt);
}


float IRModelIef::computeIef(const VOCABULARY_ADT<QString,
							 IndexTermRef>::const_iterator &vocabularyEntry)
{
	vv("Computing ief for term: " << vocabularyEntry.key());

	static const double E = mIndex.identifiers().size();

	QSet<elem_serial> elementsWithTerm;

	// We have to invoke findPosts() for each possible field


	for (int f = static_cast<int>(ElementFieldType::_Start);
		 f <= static_cast<int>(ElementFieldType::_End);
		 f = f << 1) {

		ElementFieldType type = static_cast<ElementFieldType>(f);

		// Take the posts that contains this term within this field
		QVector<IndexPost> posts;

		// findPosts is already thread-safe, no need to lock it
		mIndex.findPosts(vocabularyEntry, type, posts);

		// At this point posts contains the posts of the term within the field,
		// which may contain duplicate element id if an element.field contains
		// the same term more than once.
		// For this reason, we have to scan the posts and create another
		// set from that avoids elements duplicates

		for (const IndexPost &post : posts) {
			// Push the element id
			elementsWithTerm.insert(post.elementSerial);
		}
	}

	const double ef_t = elementsWithTerm.size();

	ASSERT(ef_t > 0, "index_handling",
		"Element frequency must be greater than 0 since the term is in the vocabulary: ",
		vocabularyEntry.key());

	ASSERT(ef_t <= E, "index_handling",
		"Element frequency must be not greater than the number of elements (",
		DEC(ef_t), " > ", ")");

	// At this point elementsWithTerm contains only the elements ids
	// in which the term occurs; the size of it is actually the ef_t
	float ief_t = FLOAT(log10(E / ef_t));

#if DEBUG
	const QString &term = vocabularyEntry.key();
	dd("E" << " = " << E);
	dd("ef(" << term << ")" << " = " << ef_t);
	dd("ief(" << term << ")" << " = " << ief_t);
#endif

	return ief_t;
}

void IRModelIef::printIefs() const
{
#if VERBOSE
	const int E = mIndex.identifiers().size();

	vv("==== IEFS ====");

	// For print the iefs sorted, put those inside a map (which automatically
	// order by key)
	QMap<float, QString> sortedIefs;

	for (auto it = mIefs.cbegin(); it != mIefs.cend(); ++it) {
		// insertMulti since more than a term can have the same score
		sortedIefs.insertMulti(it.value(), it.key());
	}

	for (auto it = sortedIefs.cbegin(); it != sortedIefs.cend(); ++it) {
		const float ief = it.key();
		// comfort: recompute ef with the inverse formula
		float ef = static_cast<float>(E / pow(10, ief));
		Q_UNUSED(ef)
		dd1("ief(" << it.value() << ") = " << ief << " | ef ~= " << ef);
	}

	vv("==== IEFS END ====");
#endif
}
