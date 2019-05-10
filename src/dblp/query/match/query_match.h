#ifndef QUERY_MATCH_H
#define QUERY_MATCH_H

#include <QSet>
#include <QtGlobal>

#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"
#include "dblp/shared/element_type/element_type.h"

enum class QueryMatchType {
	Publication,
	Venue,
	PublicationVenue
};

class QueryMatchComponent {
public:
	static const elem_serial NO_ELEMENT = ~static_cast<elem_serial>(0);

	QueryMatchComponent();
	QueryMatchComponent(const QSet<IndexMatch> &matches);

	elem_serial elementSerial() const;
	QSet<IndexMatch> matches() const;
	ElementType elementType() const;

	operator QString() const;

private:
	void finalize();

	elem_serial mSerial = NO_ELEMENT;
	QSet<IndexMatch> mMatches;
	ElementType mType;
};

class QueryMatch {
public:
	static QueryMatch forPublication(const QSet<IndexMatch> &pubMatches, float score);
	static QueryMatch forVenue(const QSet<IndexMatch> &venueMatches, float score);
	static QueryMatch forPublicationVenue(const QSet<IndexMatch> &pubMatches,
										  const QSet<IndexMatch> &venueMatches,
										  float score);

	QueryMatchType matchType() const;
	float score() const;

	QueryMatchComponent publication() const;
	QueryMatchComponent venue() const;

private:
	void finalize();

	QueryMatchType mMatchType;
	float mScore = 0;

	QueryMatchComponent mPublication;	// optional
	QueryMatchComponent mVenue;			// optional
};


// --- Hashing purpose

bool operator==(const QueryMatch &qm1, const QueryMatch &qm2);
bool operator<(const QueryMatch &qm1, const QueryMatch &qm2);
uint qHash(const QueryMatch &qm, uint seed);

// ---


#endif // QUERY_MATCH_H
