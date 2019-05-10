#include "query_match.h"
#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"

QueryMatch QueryMatch::forPublication(const QSet<IndexMatch> &pubMatches, float score)
{
	QueryMatch m;
	m.mPublicationMatches = pubMatches;
	m.mScore = score;
	m.mType = QueryMatchType::Publication;
	m.finalize();
	return m;
}

QueryMatch QueryMatch::forVenue(const QSet<IndexMatch> &venueMatches, float score)
{
	QueryMatch m;
	m.mVenueMatches = venueMatches;
	m.mScore = score;
	m.mType = QueryMatchType::Venue;
	m.finalize();
	return m;
}

QueryMatch QueryMatch::forPublicationVenue(const QSet<IndexMatch> &pubMatches,
										   const QSet<IndexMatch> &venueMatches,
										   float score)
{
	QueryMatch m;
	m.mPublicationMatches = pubMatches;
	m.mVenueMatches = venueMatches;
	m.mScore = score;
	m.mType = QueryMatchType::PublicationVenue;
	m.finalize();
	return m;
}

QueryMatchType QueryMatch::type() const
{
	return mType;
}

float QueryMatch::score() const
{
	return mScore;
}

elem_serial QueryMatch::publication() const
{
	return mPublication;
}

const QSet<IndexMatch> QueryMatch::publicationMatches() const
{
	return mPublicationMatches;
}

elem_serial QueryMatch::venue() const
{
	return mVenue;
}

const QSet<IndexMatch> QueryMatch::venueMatches() const
{
	return mVenueMatches;
}

void QueryMatch::finalize()
{
	// Ensure that all the matches within a category (pub matches and venue matches)
	// have the same elem_serial, and set it

	if (mType == QueryMatchType::Publication ||
		mType == QueryMatchType::PublicationVenue) {

		ASSERT(mPublicationMatches.size() > 0, "querying",
			   "QueryMatch has type (Publication || PublicationVenue) "
			   "but hasn't any publication;");

		foreach (IndexMatch indexMatch, mPublicationMatches) {
			ASSERT(mPublication == indexMatch.elementSerial	||
					   mPublication == NO_ELEMENT, "querying",
					   "The publication index matches for this query match don't "
					   "have the same element serial");
			mPublication = indexMatch.elementSerial;
		}
	}

	if (mType == QueryMatchType::Venue ||
		mType == QueryMatchType::PublicationVenue) {

		ASSERT(mVenueMatches.size() > 0, "querying",
			   "QueryMatch has type (Venue || PublicationVenue) "
			   "but hasn't any venue;");

		foreach (IndexMatch indexMatch, mVenueMatches) {
			ASSERT(mVenue == indexMatch.elementSerial	||
					   mVenue == NO_ELEMENT, "querying",
					   "The venue index matches for this query match don't "
					   "have the same element serial");
			mVenue = indexMatch.elementSerial;
		}
	}

	// OK: every IndexMatch within a category have the same element_serial
}

// --- Hashing purpose

bool operator==(const QueryMatch &qm1, const QueryMatch &qm2)
{
	return	qm1.publication() == qm2.publication() &&
			qm1.venue() == qm2.venue();
}

uint qHash(const QueryMatch &qm, uint seed)
{
	uint h1 = qm.publication() ? qHash(qm.publication(), seed) : 0;
	uint h2 = qm.venue() ? qHash(qm.venue(), seed) : 0;

	return h1 ^ h2;
}

// ---

bool operator<(const QueryMatch &qm1, const QueryMatch &qm2)
{
	return qm1.score() < qm2.score();
}
