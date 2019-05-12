#ifndef QUERY_H
#define QUERY_H

#include <QList>
#include <commons/log/loggable/loggable.h>
#include <dblp/query/query/models/base/query_base_part.h>
#include "dblp/query/query/models/types/query_types.h"

class Query : protected Loggable {
public:
	~Query() override;
	Query(const QString &queryString);
	operator QString() const;

	const QList<QueryBasePart *> parts() const;

protected:
	LOGGING_OVERRIDE

private:
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

	void addPart(QueryBasePart * queryPart);

	// The query is "only" its parts
	QList<QueryBasePart *> mQueryParts;
};

#endif // QUERY_H
