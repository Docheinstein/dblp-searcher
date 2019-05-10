#ifndef QUERY_MATCH_H
#define QUERY_MATCH_H

#include <QSet>
#include <QtGlobal>

#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"

enum class QueryMatchType {
	Publication,
	Venue,
	PublicationVenue
};

class QueryMatch {
public:
	static const elem_serial NO_ELEMENT = ~UINT32(0);

	static QueryMatch forPublication(const QSet<IndexMatch> &pubMatches, float score);
	static QueryMatch forVenue(const QSet<IndexMatch> &venueMatches, float score);
	static QueryMatch forPublicationVenue(const QSet<IndexMatch> &pubMatches,
										  const QSet<IndexMatch> &venueMatches,
										  float score);

	QueryMatchType type() const;
	float score() const;

	elem_serial publication() const;
	const QSet<IndexMatch> publicationMatches() const;

	elem_serial venue() const;
	const QSet<IndexMatch> venueMatches() const;

protected:
	void finalize();

	QueryMatchType mType;
	float mScore = 0;

	elem_serial mPublication = NO_ELEMENT; // automatically figured out from publication matches
	QSet<IndexMatch> mPublicationMatches;	// optional

	elem_serial mVenue = NO_ELEMENT; // automatically figured out from venue matches
	QSet<IndexMatch> mVenueMatches;			// optional
};


// --- Hashing purpose

bool operator==(const QueryMatch &qm1, const QueryMatch &qm2);
bool operator<(const QueryMatch &qm1, const QueryMatch &qm2);
uint qHash(const QueryMatch &qm, uint seed);

// ---


#endif // QUERY_MATCH_H
