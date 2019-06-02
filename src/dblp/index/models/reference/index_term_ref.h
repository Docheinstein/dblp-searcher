#ifndef INDEX_TERM_REF_H
#define INDEX_TERM_REF_H

#include <QtGlobal>

typedef struct IndexTermRefPostMeta {
	quint32 count;  // Loaded from file
	quint32 offset; // Generated accordingly to the previously loaded count(s)
} IndexTermRefPostMeta;

class IndexTermRef {
public:

	// Position of the first post in the posting list for the term (actually <art.a>)
	qint64 postingListPosition;

	// <art.a> <art.t> <art.y>
	// <jou>
	// <inc.a> <inc.t> <inc.y>
	// <inp.a> <inp.t> <inp.y>
	// <phd.a> <phd.t> <phd.y>
	// <mas.a> <mas.t> <mas.y>
	// <bok.a> <bok.t> <bok.y> <bok.p>
	// <pro.t> <pro.y> <pro.p>

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} article;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} incollection;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} inproceedings;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} phdthesis;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} mastersthesis;

	struct {
		IndexTermRefPostMeta name;
	} journal;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
		IndexTermRefPostMeta publisher;
	} book;

	struct {
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
		IndexTermRefPostMeta publisher;
	} proceedings;
};

QDebug operator<<(QDebug stream, const IndexTermRef &termref);
QDebug operator<<(QDebug stream, const IndexTermRefPostMeta &termrefpost);


#endif // INDEX_TERM_REF_H
