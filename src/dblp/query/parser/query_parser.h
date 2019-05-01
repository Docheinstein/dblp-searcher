#ifndef QUERY_PARSER_H
#define QUERY_PARSER_H

#include <dblp/query/models/query/query.h>
#include <dblp/query/models/types/query_types.h>
#include <commons/logger/logger.h>

class QueryParser {
public:
	static Query fromString(const QString &queryString);
	static QueryBasePart * newQueryPartFromToken(const QString &token);

	static QueryBasePart * newQueryPublicationPartFromToken(
			const QString &token);
	static QueryBasePart * newQueryPublicationPartFromToken(
			const QString &token,
			QueryPublicationElementCase pubElement);
	static QueryBasePart * newQueryPublicationPartFromToken(
			const QString &token,
			QueryPublicationElementCase pubElement,
			QueryPublicationFieldCase pubField);

	static QueryBasePart * newQueryVenuePartFromToken(
			const QString &token);
	static QueryBasePart * newQueryVenuePartFromToken(
			const QString &token,
			QueryVenueElementCase venueElement,
			QueryVenueFieldCase venueField);

private:
	static Logger L;
};

#endif // QUERY_PARSER_H
