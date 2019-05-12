#include "query_resolver.h"
#include "math.h"
#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"
#include "commons/const/const.h"
#include "commons/config/config.h"
#include "commons/profiler/profiler.h"

LOGGING(QueryResolver, true);

QueryResolver::QueryResolver(IRModel *irmodel)
{
	mIrModel = irmodel;
}

IRModel *QueryResolver::irModel()
{
	return mIrModel;
}

QVector<QueryMatch> QueryResolver::resolveQuery(const Query &query) {
	PROF_FUNC_BEGIN1

	IndexHandler *index = mIrModel->index();

	struct ScoredIndexElementMatches {
		QVector<IndexMatch> matches; // the matches should have the same serial
		float score;
	};

	QHash<elem_serial, ScoredIndexElementMatches> scoredPubs;
	QHash<elem_serial, ScoredIndexElementMatches> scoredVenues;

	QVector<QueryMatch> queryMatches;

	QVector<IndexMatch> publicationMatches;
	QVector<IndexMatch> venueMatches;

	PROF_BEGIN2(resolveQueryFind)

	// Find matches (posts)

	foreach (QueryBasePart *part, query.parts()) {

		vv1("Resolving part of query: " << *part);

		foreach (QString macroToken, part->tokens()) {

			vv2("Resolving subpart for token: " << macroToken);

			// Figure out if we are treating publications or venue (or both, for All)
			ElementFieldTypes searchFields = part->elementFieldTypes();
			int searchFieldsFlags = INT(searchFields);

			// Check if the flags belong to publication types or venue types
			// We have to do this double direction check for tollerate both
			// a superset or a subset of the flags
			bool isPublication =
					searchFields.testFlag((ElementFieldType::Publication)) ||
					((searchFieldsFlags &
					 INT(ElementFieldType::Publication)) == searchFieldsFlags);
			bool isVenue =
					searchFields.testFlag((ElementFieldType::Venue)) ||
					((searchFieldsFlags &
					 INT(ElementFieldType::Venue)) == searchFieldsFlags);


			// Potentially we could treat the case All for global searches,
			// thus perform two calls to findElements()

			if (!isPublication && !isVenue) {
				ee("The field type flags (" << searchFieldsFlags <<
				   ") do not belong neither to publication or venue, WTF?");

				ee("Search flags     : " << searchFields << " ("
				   << BIN(searchFieldsFlags) << ")");
				ee("Publication flags: " << searchFields << " ("
				   << BIN(INT(ElementFieldType::Publication)) << ")");
				ee("Venue flags      : " << searchFields << " ("
				   << BIN(INT(ElementFieldType::Venue)) << ")");

				QUIT("Unknown field types (doesn't belong neither to publication nor venue");
			}

			if (isPublication) {
				vv3("Treating search (" << macroToken << ") for PUBLICATION type");

				// Take just the publications; thus put searchFields in AND
				// with the Publication flags (this is needed for ensure
				// that the All type takes just the publications)
				// Obviously we cannot use just ElementFieldType::Publication,
				// otherwise we would throw away any other filter of the query part

				index->findMatches(macroToken,
								searchFields & ElementFieldType::Publication,
								publicationMatches);
			}

			if (isVenue) {
				vv3("Treating search (" << macroToken << ") for VENUE type");

				// Take just the venues; thus put searchFields in AND
				// with the Venue flags (this is needed for ensure
				// that the All type takes just the venues)
				// Obviously we cannot use just ElementFieldType::Venue,
				// otherwise we would throw away any other filter of the query part

				index->findMatches(macroToken,
								searchFields & ElementFieldType::Venue,
								venueMatches);
			}
		}
	}

	PROF_END(resolveQueryFind)

	// Compute score of elements among the matches

	PROF_BEGIN2(resolveQueryScoreComputing)

	auto computeElementsScore = [this](
				QVector<IndexMatch> &matches,
				QHash<elem_serial, ScoredIndexElementMatches> &scoredMatches) {

		// Foreach match
		for (const IndexMatch &match : matches) {

			// Retrieve the current score for this element, if it doesn't exist
			// insert a new one

			if (!scoredMatches.contains(match.elementSerial)) {
				dd4("Creating ScoredIndexMatches for element = " << efMatch.elementSerial);
				// With the no match (for now, than this efMatch will
				// be pushed) and score = 0 (which will
				// be increased later)
				scoredMatches.insert(match.elementSerial, {{}, 0});
			}

			// Take the (existing) reference
			auto scoredMatchIt = scoredMatches.find(match.elementSerial);

			ASSERT(scoredMatchIt != scoredMatches.end(), "querying",
				"ScoredIndexMatches not found; is this a programming error?");

			ScoredIndexElementMatches &scoredMatches = scoredMatchIt.value();

			// Push this match
			scoredMatches.matches.append(match);

			float matchPartialScore = 0;

			// For each matched token inside this match, retrieve the ief
			// and add it to the global score of the associated element
			for (const QString &token : match.matchedTokens) {
				float ief = mIrModel->termScore(token);
				dd4("[e" << efMatch.elementSerial << "] ief(" << queryPartTerm << ") = " << ief );
				matchPartialScore += ief;
			}

			// Moreover, I would like to enhance the phrasal query, for this reason
			// the score of a phrasal is the sum of the if.ief of its tokens plus a bonus
			// defined as C^(len(tokens)), so that if the phrase contain more tokens
			// the bonus increase

			if (match.matchedTokens.size() > 1) {
				float phraseBonus = pow(
					mIrModel->bonusFactorPerPhraseTerm(),
					FLOAT(match.matchedTokens.size() - 1) // -1 for avoid to bonus non phrasals
				);

				vv2("Giving bonus of " << phraseBonus
				   << " to phrase: " << queryPartTerms.join(" "));
				matchPartialScore *= phraseBonus;
			}

#if DEBUG
			QString s = DEC(match.elementSerial) + " += " +
						FLT(matchPartialScore);
#endif
			// Add the score to the this scoredMatches (for a single element)
			scoredMatches.score += matchPartialScore;
#if DEBUG
			s += FLT(scoredMatches.score);

			dd3(s);
#endif
		}
	};

	computeElementsScore(publicationMatches, scoredPubs);
	computeElementsScore(venueMatches, scoredVenues);

	PROF_END(resolveQueryScoreComputing)

	QSet<elem_serial> crossReferredVenues;

	PROF_BEGIN2(crossrefsCheck)

	for (auto it = scoredPubs.begin(); it != scoredPubs.end(); it++) {

		const elem_serial pubSerial = it.key();
		const ScoredIndexElementMatches &scoredPubMatches = it.value();

		// PUB+VENUE match

		// We have to meet two condition for declare a pub+venue, match
		// first of all the crossref must exists
		// within the index, furthermore we should have the venue between our
		// venue matches to consider this a pub+venue match.
		bool crossrefMatch = false;
		elem_serial crossrefSerial;

		vv1("Checking crossref existence for element = " << pubSerial);

		// Retrieve the crossref
		if (mIrModel->index()->crossref(pubSerial, crossrefSerial)) {
			// Crossref found, check if we have crossrefSerial among
			// our matches venues

			dd2("Crossref found; " << pubSerial << " => " << crossrefSerial);

			auto crossrefVenueIt = scoredVenues.find(crossrefSerial);

			if (crossrefVenueIt != scoredVenues.end()) {
				// We have both a publication and the crossreffed venue among
				// our pub matches and venue matches, enhance the score
				// and push a pub+venue match
				dd3("Crossref found and venue is among matched; "
					"this is actually a pub+venue match");

				crossrefMatch = true; // do not push the publication
									  // by itself later, but as a pub+venue now

				crossReferredVenues.insert(crossrefSerial);

				const ScoredIndexElementMatches &scoredVenueMatches = *crossrefVenueIt;

				// Compute the score (at least the sum)

				dd3("[" << pubSerial << "] score = " << scoredPubMatches.score);
				dd3("[" << crossrefSerial << "] score = " << scoredVenueMatches.score);
				dd3("Bonus factor = " << mIrModel->bonusFactorForPublicationVenueMatch());

				float enhancedScore =
						(scoredPubMatches.score + scoredVenueMatches.score) *
						mIrModel->bonusFactorForPublicationVenueMatch();

				vv2("Pushing pub+venue match for [" << pubSerial << "]" <<
					" => [" << crossrefSerial << "] - score = " << enhancedScore);

				queryMatches.append(QueryMatch::forPublicationVenue(
					scoredPubMatches.matches, scoredVenueMatches.matches,
					enhancedScore));
			}
			// else: the crossreffed venue is not among the matched venues
		}
		// else: crossref not even found within the index

		// PUB match

		if (!crossrefMatch) {
			// Crossref not found, just push the publication to the matches
			// using as query match score the publication score
			dd2("Pubication to venue match crossref not satisfied");

			float score = scoredPubMatches.score *
					mIrModel->bonusFactorForPublicationMatch();

			vv2("Pushing pub match for [" << pubSerial <<
				"] - score = " << score);

			queryMatches.append(QueryMatch::forPublication(
									scoredPubMatches.matches, score));
		}
	}

	// VENUE matches (the venue not crossreferred yet)

	for (auto it = scoredVenues.begin(); it != scoredVenues.end(); it++) {
		const elem_serial venueSerial = it.key();

		if (crossReferredVenues.contains(venueSerial)) {
			dd1("Venue [" << venueSerial << "] is already part of a pub+venue; ignoring it");
			continue;
		}

		const ScoredIndexElementMatches &scoredVenueMatches = it.value();

		// Venue do not appear in any pub+venue, push it

		float score = scoredVenueMatches.score *
				mIrModel->bonusFactorForVenueMatch();

		vv1("Pushing venue match for [" << venueSerial << "] - score = " << score);
		queryMatches.append(QueryMatch::forVenue(
								scoredVenueMatches.matches, score));
	}

	PROF_END(crossrefsCheck)

	PROF_BEGIN2(sortQueryMatches)

	// Finally, sort the query matches by score (reverse)
	std::sort(queryMatches.rbegin(), queryMatches.rend());

	PROF_END(sortQueryMatches)

	PROF_FUNC_END

	return queryMatches;
}
