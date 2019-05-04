#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include <commons/adt/type/string_type.h>

// BASE

class QueryElementType : public StringType {
public:
	QueryElementType(const char *string);
};

class QueryFieldType : public StringType {
public:
	QueryFieldType(const char *string);
};

// PUBLICATION

class QueryPublicationElementType : public QueryElementType {
public:
	QueryPublicationElementType(const char *s) : QueryElementType (s) {}

	static QueryPublicationElementType * PUBLICATION;
	static QueryPublicationElementType * ARTICLE;
	static QueryPublicationElementType * INCOLLECTION;
	static QueryPublicationElementType * INPROCEEDINGS;
	static QueryPublicationElementType * PHDTHESIS;
	static QueryPublicationElementType * MASTERTHESIS;
};

typedef QueryPublicationElementType* QueryPublicationElementCase;


class QueryPublicationFieldType : public QueryFieldType {
public:
	QueryPublicationFieldType(const char *s) : QueryFieldType(s) {}

	static QueryPublicationFieldType *AUTHOR;
	static QueryPublicationFieldType *TITLE;
	static QueryPublicationFieldType *YEAR;
};

typedef QueryPublicationFieldType* QueryPublicationFieldCase;

// VENUE

class QueryVenueElementType : public QueryElementType {
public:
	QueryVenueElementType(const char *s) : QueryElementType(s) {}

	static QueryVenueElementType *VENUE;
};

typedef QueryVenueElementType* QueryVenueElementCase;


class QueryVenueFieldType : public QueryFieldType {
public:
	QueryVenueFieldType(const char *s) : QueryFieldType(s) {}

	static QueryVenueFieldType *TITLE;
	static QueryVenueFieldType *PUBLISHER;
};

typedef QueryVenueFieldType* QueryVenueFieldCase;


#endif // QUERY_TYPES_H
