#ifndef INDEX_TERM_H
#define INDEX_TERM_H

#include <QList>
#include "dblp/index/models/post/index_post.h"

typedef QList<IndexPost> Posts;

class IndexTerm {
public:
	int debug_refCount = 0;

	struct {
		Posts author;
		Posts title;
		Posts year;
	} article;

	// Journal?

	struct {
		Posts author;
		Posts title;
		Posts year;
		Posts booktitle;
	} incollection;

	struct {
		Posts author;
		Posts title;
		Posts year;
		Posts publisher;
	} book;

	struct {
		Posts author;
		Posts title;
		Posts year;
		Posts booktitle;
	} inproceedings;

	struct {
		Posts author;
		Posts title;
		Posts year;
		Posts publisher;
		Posts booktitle;
	} proceedings;

	struct {
		Posts author;
		Posts title;
		Posts year;
	} phdthesis;

	struct {
		Posts author;
		Posts title;
		Posts year;
	} masterthesis;
};

#endif // INDEX_TERM_H
