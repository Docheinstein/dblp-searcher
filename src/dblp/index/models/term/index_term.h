#ifndef INDEX_TERM_H
#define INDEX_TERM_H

#include <QList>
#include "dblp/index/models/post/index_post.h"

typedef QList<IndexPost> IndexPosts;

class IndexTerm {
public:
	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts  year;
	} article;

	// Virtual since journal actually doesn't exists as element within the .xml
	struct {
		IndexPosts name;
	} journal;

	// Journal?

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
		IndexPosts booktitle;
	} incollection;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
		IndexPosts publisher;
	} book;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
		IndexPosts booktitle;
	} inproceedings;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
		IndexPosts publisher;
		IndexPosts booktitle;
	} proceedings;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
	} phdthesis;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
	} mastersthesis;

	// STATS/DEBUG
	struct {
		quint64 postsCount;
	} stats;
};

#endif // INDEX_TERM_H
