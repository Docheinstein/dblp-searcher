#ifndef QUERY_TYPES_H
#define QUERY_TYPES_H

#include <commons/adt/type/string_type.h>

// PUBLICATION

class QueryPublicationElementType : public StringType {
public:
	QueryPublicationElementType(const char *s) : StringType(s) {}

	static QueryPublicationElementType * PUBLICATION;
	static QueryPublicationElementType * ARTICLE;
	static QueryPublicationElementType * INCOLLECTION;
	static QueryPublicationElementType * INPROCEEDINGS;
	static QueryPublicationElementType * PHDTHESIS;
	static QueryPublicationElementType * MASTERTHESIS;
};

typedef QueryPublicationElementType* QueryPublicationElementCase;


class QueryPublicationFieldType : public StringType {
public:
	QueryPublicationFieldType(const char *s) : StringType(s) {}

	static QueryPublicationFieldType *AUTHOR;
	static QueryPublicationFieldType *TITLE;
	static QueryPublicationFieldType *YEAR;
};

typedef QueryPublicationFieldType* QueryPublicationFieldCase;

// VENUE

class QueryVenueElementType : public StringType {
public:
	QueryVenueElementType(const char *s) : StringType(s) {}

	static QueryVenueElementType *VENUE;
};

typedef QueryVenueElementType* QueryVenueElementCase;


class QueryVenueFieldType : public StringType {
public:
	QueryVenueFieldType(const char *s) : StringType(s) {}

	static QueryVenueFieldType *TITLE;
	static QueryVenueFieldType *PUBLISHER;
};

typedef QueryVenueFieldType* QueryVenueFieldCase;


#endif // QUERY_TYPES_H
