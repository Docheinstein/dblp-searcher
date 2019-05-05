#include "math.h"
#include "commons/globals/globals.h"
#include "ir_model_ief.h"
#include <dblp/index/models/reference/index_term_ref.h>
#include "dblp/index/handler/index_handler.h"

static const float TERM_NOT_FOUND = -1;

// REMOVE ME
//#include <QThread>

Logger IRModelIef::L = Logger::forClass("IRModel");

IRModelIef::~IRModelIef() {}

IRModelIef::IRModelIef(IndexHandler *indexHandler) : IRModel(indexHandler)
{

}

void IRModelIef::init(bool lazy)
{
	// Calling computeIefs() now will speed up any further call to termScore()
	// but may require some time, for this reason the computing is parametrized
	// at can be done either now or at runtime
	if (lazy) {
		dd("Not computing iefs now since lazy initialization has been required");

		// Emit signals any way
		emit initStarted();
		emit initEnded();
		return;
	}

	// Compute all the iefs now (may require some time)
	computeIefs();
	debug_printIefs();
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

	// Ief has never been computed for the term, compute it, put in the map
	// and return it
	ief = computeIef(term);
	mIefs.insert(term, ief);

	return ief;
}

float IRModelIef::bonusFactorPerPhraseTerm()
{
	return 1.15f;
}

void IRModelIef::computeIefs()
{
	ii("Computing inverse element frequencies...");

	emit initStarted();

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

	const QMap<QString, IndexTermRef> &vocabulary = mIndex->vocabulary();

	const qint64 vocabularySize = vocabulary.size();

	int i = 0;

	for (auto it = vocabulary.begin(); it != vocabulary.end(); it++, i++) {
		const QString &term = it.key();
		const IndexTermRef &ref = it.value();

		if (term.isEmpty())
			continue;

//		ii("Computing ief of '" << term << "'");

		float ief = computeIef(term, ref);

		// Finally put the ief_t inside the hash table
		mIefs.insert(term, ief);

		// Notify the current progress
		double progress = static_cast<double>(i) / vocabularySize;
//		ii("IRModel Progress: " << progress);

		emit initProgress(progress);
	}

	ii("Iefs computing finished");

	emit initEnded();
}

float IRModelIef::computeIef(const QString &term)
{
	auto termRef = mIndex->vocabulary().find(term);

	// Term not present in the vocabulary
	if (termRef == mIndex->vocabulary().end())
		return TERM_NOT_FOUND;

	// Really compute the ief
	return computeIef(term, termRef.value());
}


float IRModelIef::computeIef(const QString &term, const IndexTermRef &ref)
{
	static const double E = mIndex->keys().size();

	// We have to invoke loadPosts() for each possible field

	QSet<quint32> elementsWithTerm;

	for (int f = static_cast<int>(ElementFieldType::_Start);
		 f <= static_cast<int>(ElementFieldType::_End);
		 f = f << 1) {

//		ii("For field: " << f);
		ElementFieldType type = static_cast<ElementFieldType>(f);
//		ii("H1");

		// Take the posts that contains this term within this field
		QSet<IndexPost> posts;
//		ii("H2");

		mIndex->findPosts(term, ref, type, posts);
//		ii("H3");

		// At this point posts contains the posts of the term within the field,
		// which may contain duplicate element id if an element.field contains
		// the same term more than once.
		// For this reason, we have to scan the posts and create another
		// set from that avoids elements duplicates

		int i = 0;
//		ii("H4");

		foreach (IndexPost post, posts) {
//			ii("Post num: " << i++);

			// Push the element id
			elementsWithTerm.insert(post.elementId);
		}
//		ii("H5");

	}
//	ii("H6");

	const double ef_t = elementsWithTerm.size();

	Q_ASSERT_X(ef_t > 0, "index_handling",
		QString(
			QString("Element frequency must be greater than 0 since the term is in the vocabulary: ") +
			QString(term)
		).toStdString().c_str());

	Q_ASSERT_X(ef_t <= E, "index_handling",
		QString(
			QString("Element frequency must be not greater than the number of elements (") +
			DEC(ef_t) + " > " + DEC(E) + ")"
		).toStdString().c_str());

	// At this point elementsWithTerm contains only the elements ids
	// in which the term occurs; the size of it is actually the ef_t
	float ief_t = static_cast<float>(log10(E / ef_t));

//	ii("E" << " = " << E);
//	ii("ef(" << term << ")" << " = " << ef_t);
//	ii("ief(" << term << ")" << " = " << ief_t);

	return ief_t;
}

void IRModelIef::debug_printIefs()
{
	const int E = mIndex->keys().size();

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
