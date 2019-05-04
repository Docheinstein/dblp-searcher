#include "ir_model.h"
#include "math.h"

Logger IRModel::L = Logger::forClass("IRModel");

IRModel::IRModel(IndexHandler *indexHandler)
{
	mIndex = indexHandler;
	init();
}

float IRModel::ief(const QString &term)
{
	return mIefs.value(term, 0);
}

IndexHandler *IRModel::index()
{
	return mIndex;
}

void IRModel::init()
{
	computeIefs();
	debug_printIefs();
}


void IRModel::computeIefs()
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

	const QMap<QString, IndexTermRef> &vocabulary = mIndex->vocabulary();
	const double E = mIndex->keys().size();

	for (auto it = vocabulary.begin(); it != vocabulary.end(); it++) {
		QString term = it.key();
		const IndexTermRef &ref = it.value();

		vv("Computing ief of " << term);

		// We have to invoke loadPosts() for each possible field

		QSet<quint32> elementsWithTerm;

		for (int f = static_cast<int>(ElementFieldType::_Start);
			 f <= static_cast<int>(ElementFieldType::_End);
			 f = f << 1) {
			ElementFieldType type = static_cast<ElementFieldType>(f);

			// Take the posts that contains this term within this field
			QSet<IndexPost> posts;
			mIndex->findPosts(term, ref, type, posts);

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

void IRModel::debug_printIefs()
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
