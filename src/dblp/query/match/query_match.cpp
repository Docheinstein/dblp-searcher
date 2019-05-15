#include "query_match.h"
#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"
#include "dblp/shared/element_type/element_type.h"

QueryMatchComponent::QueryMatchComponent() {}

QueryMatchComponent::QueryMatchComponent(const QVector<IndexMatch> &matches)
{
	mMatches = matches;
	finalize();
}

elem_serial QueryMatchComponent::elementSerial() const
{
	return mSerial;
}

QVector<IndexMatch> QueryMatchComponent::matches() const
{
	return mMatches;
}

ElementType QueryMatchComponent::elementType() const
{
	return mType;
}

void QueryMatchComponent::finalize()
{
	// The constructor takes only some matches, we have to ensure a couple of things
	// 1) All the matches should have the same element serial
	// 2) All the matches should belong to the same element type (e.g. Article)

	ASSERT(mMatches.size() > 0, "querying",
		   "QueryMatchComponent initialized without matches; "
		   "at least 1 is required");

	// Take those from the first
	auto matchesIt = mMatches.begin();
	mSerial = matchesIt->elementSerial;
	mType = elementTypeFromElementFieldType(matchesIt->fieldType);

	// Ensure that every other matches have the same serial and type
	matchesIt++;

	for (; matchesIt != mMatches.end(); matchesIt++) {
		// 1) Serial check
		ASSERT(	mSerial == matchesIt->elementSerial,
			   "querying",
			   "The index matches for this query match component don't "
			   "have the same element serial (", mSerial, " != ",
				matchesIt->elementSerial);

		// 2) Element type check
		ASSERT(mType == elementTypeFromElementFieldType(matchesIt->fieldType),
			   "querying",
			   "The index matches for this query match component don't "
			   "have the same element type (", elementTypeString(mType), " != ",
			   elementFieldTypeString(matchesIt->fieldType));
	}

	// OK: every IndexMatch within a category have the same element_serial
	// and belong to the same element type
}

QueryMatchComponent::operator QString() const
{
	return QString("{serial = ") + DEC(mSerial) + "; type = " +
			elementTypeString(mType) + "; # matches = " +
			DEC(mMatches.size()) + "}";
}


QueryMatch QueryMatch::forPublication(const QVector<IndexMatch> &pubMatches, float score)
{
	QueryMatch match;
	match.mPublication = QueryMatchComponent(pubMatches);
	match.mScore = score;
	match.mMatchType = QueryMatchType::Publication;
	match.finalize();
	return match;
}

QueryMatch QueryMatch::forVenue(const QVector<IndexMatch> &venueMatches, float score)
{
	QueryMatch match;
	match.mVenue = QueryMatchComponent(venueMatches);
	match.mScore = score;
	match.mMatchType = QueryMatchType::Venue;
	match.finalize();
	return match;
}

QueryMatch QueryMatch::forPublicationVenue(const QVector<IndexMatch> &pubMatches,
										   const QVector<IndexMatch> &venueMatches,
										   float score)
{
	QueryMatch match;
	match.mPublication = QueryMatchComponent(pubMatches);
	match.mVenue = QueryMatchComponent(venueMatches);
	match.mScore = score;
	match.mMatchType = QueryMatchType::PublicationVenue;
	match.finalize();
	return match;
}

QueryMatchType QueryMatch::matchType() const
{
	return mMatchType;
}

float QueryMatch::score() const
{
	return mScore;
}

QueryMatchComponent QueryMatch::publication() const
{
	return mPublication;
}

QueryMatchComponent QueryMatch::venue() const
{
	return mVenue;
}

void QueryMatch::finalize()
{
	// Check the the underlying element type of the components matches
	// with the matchType
	if (mMatchType == QueryMatchType::Publication ||
		mMatchType == QueryMatchType::PublicationVenue) {
		int pubType = INT(mPublication.elementType());
		ASSERT((pubType & INT(ElementType::Publication)) == pubType,
				"querying",
				"Underlying query match publication component matches "
				"type do not satisfy declared query match type");

	}

	if (mMatchType == QueryMatchType::Venue ||
		mMatchType == QueryMatchType::PublicationVenue) {
		int venueType = INT(mVenue.elementType());
		ASSERT((venueType & INT(ElementType::Venue)) == venueType,
				"querying",
				"Underlying query match venue omponent matches "
				"type do not satisfy declared query match type");
	}
}
