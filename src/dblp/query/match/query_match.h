#ifndef QUERY_MATCH_H
#define QUERY_MATCH_H

#include <QVector>
#include <QtGlobal>

#include "dblp/shared/defs/defs.h"
#include "dblp/shared/element_type/element_type.h"
#include "dblp/index/models/match/index_match.h"
#include "commons/globals/globals.h"
#include "commons/config/config.h"

enum class QueryMatchType {
	Publication,
	Venue,
	PublicationVenue
};

QString queryMatchTypeString(QueryMatchType type);

class QueryMatchComponent {
public:
	static const elem_serial NO_ELEMENT = ~static_cast<elem_serial>(0);

	QueryMatchComponent();
	QueryMatchComponent(const QVector<IndexMatch> &matches);

	elem_serial elementSerial() const;
	ElementType elementType() const;
	QVector<IndexMatch> matches() const;

	operator QString() const;

private:
	void finalize();

	elem_serial mSerial = NO_ELEMENT;
	ElementType mType;
	QVector<IndexMatch> mMatches;
};

class QueryMatch {
public:
	static QueryMatch forPublication(const QVector<IndexMatch> &pubMatches, float score);
	static QueryMatch forVenue(const QVector<IndexMatch> &venueMatches, float score);
	static QueryMatch forPublicationVenue(const QVector<IndexMatch> &pubMatches,
										  const QVector<IndexMatch> &venueMatches,
										  float score);

	QueryMatchType matchType() const;
	float score() const;

	QueryMatchComponent publication() const;
	QueryMatchComponent venue() const;

	operator QString() const;

private:
	void finalize();

	QueryMatchType mMatchType;
	float mScore = 0;

	QueryMatchComponent mPublication;	// optional
	QueryMatchComponent mVenue;			// optional
};


// --- Hashing purpose

bool operator<(const QueryMatch &qm1, const QueryMatch &qm2);
bool operator==(const QueryMatch &qm1, const QueryMatch &qm2);
uint qHash(const QueryMatch &qm);

// ---


#endif // QUERY_MATCH_H
