#ifndef QUERY_H
#define QUERY_H

#include <QVector>
#include "commons/log/loggable/loggable.h"
#include "dblp/query/query/models/types/query_types.h"

class QueryBasePart;

class Query : protected Loggable {
public:
	~Query() override;
	Query(const QString &queryString);
	operator QString() const;

	const QVector<QueryBasePart *> parts() const;

protected:
	LOGGING_OVERRIDE

private:
	// We have to use QueryBasePart pointer everywhere since we cannot
	// have a vector of non pointers of abstract classes

	static QueryBasePart * newQueryPartFromToken(
			   const QString &token);

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
			QueryVenueElementCase venueElement);
	static QueryBasePart * newQueryVenuePartFromToken(
			const QString &token,
			QueryVenueElementCase venueElement,
			QueryVenueFieldCase venueField);

	static QueryBasePart * newQueryPartFromToken(
			const QString &token,
			QueryElementType * element,
			QueryFieldType * field,
			QueryBasePart *(queryPartCreator)(QueryElementType *el, QueryFieldType *fl));

	static QString queryPartSearchPattern(const QString &element, const QString &field);

	void addPart(QueryBasePart * queryPart);

	// The query is "only" its parts
	QVector<QueryBasePart *> mQueryParts;
};

#endif // QUERY_H
