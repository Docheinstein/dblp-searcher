#ifndef QUERY_MATCH_H
#define QUERY_MATCH_H

#include <QVector>
#include <QtGlobal>

#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"
#include "dblp/shared/element_type/element_type.h"
#include "commons/config/config.h"

enum class QueryMatchType {
	Publication,
	Venue,
	PublicationVenue
};

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

private:
	void finalize();

	QueryMatchType mMatchType;
	float mScore = 0;

	QueryMatchComponent mPublication;	// optional
	QueryMatchComponent mVenue;			// optional
};


// --- Hashing purpose

inline bool operator==(const QueryMatch &qm1, const QueryMatch &qm2)
{
	if (qm1.matchType() != qm2.matchType())
		return false;

	if (qm1.matchType() == QueryMatchType::Publication ||
		qm1.matchType() == QueryMatchType::PublicationVenue) {
		if (qm1.publication().elementSerial() != qm2.publication().elementSerial())
			return false;
	}

	if (qm1.matchType() == QueryMatchType::Venue ||
		qm1.matchType() == QueryMatchType::PublicationVenue) {
		if (qm1.venue().elementSerial() != qm2.venue().elementSerial())
			return false;
	}

	return true;
}

inline uint qHash(const QueryMatch &qm)
{
	static quint32 SERIAL_ENLARGER =
			UINT_MAX / Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD;

	return	qHash(qm.publication().elementSerial() * SERIAL_ENLARGER) ^
			qHash(qm.venue().elementSerial() * SERIAL_ENLARGER);
}


inline bool operator<(const QueryMatch &qm1, const QueryMatch &qm2)
{
	return qm1.score() < qm2.score();
}


// ---


#endif // QUERY_MATCH_H
