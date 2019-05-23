#ifndef INDEX_TERM_H
#define INDEX_TERM_H

#include <QVector>
#include "dblp/index/models/post/index_post.h"

typedef QVector<IndexPost> IndexPosts;

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
	} inproceedings;

	struct {
		IndexPosts author;
		IndexPosts title;
		IndexPosts year;
		IndexPosts publisher;
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
