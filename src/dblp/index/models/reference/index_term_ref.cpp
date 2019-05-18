#include "index_term_ref.h"
#include "commons/config/config.h"

#include <QDebug>

QDebug operator<<(QDebug stream, const IndexTermRef &tr) {
	QDebugStateSaver saver(stream);
	stream << endl <<
		"POS: " << tr.postingListPosition << endl <<
	   "art (a, t, y):\t" <<
		  tr.article.author << ",\t" <<
		  tr.article.title << ",\t" <<
		  tr.article.year << endl <<
	   "inc (a, t, y):\t" <<
		  tr.incollection.author << ",\t" <<
		  tr.incollection.title << ",\t" <<
		  tr.incollection.year << ",\t" << endl <<
//		  tr.incollection.booktitle << endl <<
	   "inp (a, t, y):\t" <<
		  tr.inproceedings.author << ",\t" <<
		  tr.inproceedings.title << ",\t" <<
		  tr.inproceedings.year << ",\t" << endl <<
//		  tr.inproceedings.booktitle << endl <<
	   "phd (a, t, y):\t" <<
		  tr.phdthesis.author << ",\t" <<
		  tr.phdthesis.title << ",\t" <<
		  tr.phdthesis.year << endl <<
	   "mas (a, t, y):\t" <<
		  tr.masterthesis.author << ",\t" <<
		  tr.masterthesis.title << ",\t" <<
		  tr.masterthesis.year << endl <<
	   "bok (a, t, y, p):\t" <<
		  tr.book.author << ",\t" <<
		  tr.book.title << ",\t" <<
		  tr.book.year << ",\t" <<
		  tr.book.publisher << endl <<
		"pro (t, y, p):\t" <<
		  tr.proceedings.title << ",\t" <<
		  tr.proceedings.year << ",\t" <<
		  tr.proceedings.publisher;
//		  tr.proceedings.booktitle;

	return stream;
}

QDebug operator<<(QDebug stream, const IndexTermRefPostMeta &termrefpost)
{
	QDebugStateSaver saver(stream);
	stream << "(# " << termrefpost.count << "; ... " << termrefpost.offset << ")";
	return stream;
}

