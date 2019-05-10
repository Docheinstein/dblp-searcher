#ifndef INDEX_TERM_REF_H
#define INDEX_TERM_REF_H

#include <QtGlobal>

typedef struct IndexTermRefPostMeta {
	quint32 count;  // Loaded (will be quint32)
	quint32 offset; // Generated
} IndexTermRefPostMeta;

class IndexTermRef {
public:

	// Position of the first post in the posting list for the term (actually <art.a>
	qint64 postingListPosition;

	// <art.a> <art.t> <art.y>
	// <jou>
	// <inc.a> <inc.t> <inc.y> <inc.b>
	// <inp.a> <inp.t> <inp.y> <inp.b>
	// <phd.a> <phd.t> <phd.y>
	// <mas.a> <mas.t> <mas.y>
	// <bok.a> <bok.t> <bok.y> <bok.p>
	// <pro.t> <pro.y> <pro.p> <pro.b>

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
	} article;

	struct {
		IndexTermRefPostMeta name;
	} journal;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
		IndexTermRefPostMeta booktitle;
	} incollection;

	struct {
		IndexTermRefPostMeta author;
		IndexTermRefPostMeta title;
		IndexTermRefPostMeta year;
		IndexTermRefPostMeta booktitle;
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
	} masterthesis;

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
		IndexTermRefPostMeta booktitle;
	} proceedings;

//	int positionOf();
};

QDebug operator<<(QDebug stream, const IndexTermRef &termref);
QDebug operator<<(QDebug stream, const IndexTermRefPostMeta &termrefpost);


#endif // INDEX_TERM_REF_H
