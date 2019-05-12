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

QList<QueryMatch> QueryResolver::resolveQuery(const Query &query)
{
	PROF_FUNC_BEGIN1

	/*
	 * The query is resolved with the following logic
	 * x) Retr pubs
	 * x) Retr venues
	 * x) Compute scores tf.ief, bonussing phrases
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 *
	 */

	struct ScoredIndexMatches {
		QSet<IndexMatch> matches; // the matches should have the same serial
		float score;
	};

	QHash<elem_serial, ScoredIndexMatches> scoredPubs;
	QHash<elem_serial, ScoredIndexMatches> scoredVenues;

	QList<QueryMatch> matches;

	IndexHandler *index = mIrModel->index();

	vv("Going to resolve query: " << endl << query);
	vv1("Composed by " << query.parts().size() << " parts");

//	QSet<ElementFieldMatch> pubMatches;
//	QSet<ElementFieldMatch> venueMatches;

	// Resolve each query part (the query part are ORred)
	foreach (QueryBasePart *part, query.parts()) {

		PROF_BEGIN2(resolveQueryPart)

		vv1("Resolving part of query: " << *part);
		// For each bunch of tokens of the part (which might be either
		// a simple word or phrase) we have to perform findElements();

		QHash<QString, QSet<IndexMatch>> partPubMatches;
		QHash<QString, QSet<IndexMatch>> partVenueMatches;

		foreach (QString macroToken, part->tokens()) {

			PROF_BEGIN3(resolveQueryPartMacroToken)

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
				QSet<IndexMatch> partPubMatchesSet;

				index->findMatches(macroToken,
									searchFields & ElementFieldType::Publication,
									partPubMatchesSet);

				partPubMatches.insert(macroToken, partPubMatchesSet);
			}

			if (isVenue) {
				vv3("Treating search (" << macroToken << ") for VENUE type");

				// Take just the venues; thus put searchFields in AND
				// with the Venue flags (this is needed for ensure
				// that the All type takes just the venues)
				// Obviously we cannot use just ElementFieldType::Venue,
				// otherwise we would throw away any other filter of the query part
				QSet<IndexMatch> partVenueMatchesSet;

				index->findMatches(macroToken,
									searchFields & ElementFieldType::Venue,
									partVenueMatchesSet);

				partVenueMatches.insert(macroToken, partVenueMatchesSet);

			}

			PROF_END(resolveQueryPartMacroToken)
		}

		// Compute the score for each retrieved element (pub or venue)
		// and add it to the current score of that element

		PROF_BEGIN2(resolveQueryPartScoreComputing)

		vv2("Computing score for query part: " << *part);

		auto elementScoreCalculator = [this](
				QHash<QString, QSet<IndexMatch>> &partMatches,
				QHash<elem_serial, ScoredIndexMatches> &matchesScores) {

			// Foreach query part
			for (auto it = partMatches.begin(); it != partMatches.end(); it++) {
				const QString &macroToken = it.key();
				const QSet<IndexMatch> &matches = it.value();

				// Split the macro token (which might be a word or a phrase) into
				// simple terms for which we know the ief

				QStringList queryPartTerms =
						macroToken.split(Const::Dblp::Query::TOKENS_SPLITTER,
						QString::SplitBehavior::SkipEmptyParts);

				// Foreach match of the matches found for the macro token
				foreach (IndexMatch efMatch, matches) {

					dd3("Foreach efMatch; current: " << efMatch);
					// Push a ScoredIndexMatches if it doesn't exists yet

					if (!matchesScores.contains(efMatch.elementSerial)) {
						dd4("Creating ScoredIndexMatches for element = " << efMatch.elementSerial);
						// With the no match (for now, than this efMatch will
						// be pushed) and score = 0 (which will
						// be increased later)
						matchesScores.insert(efMatch.elementSerial, {{}, 0});
					}

					// Take the (existing) reference
					auto scoredMatchIt = matchesScores.find(efMatch.elementSerial);

					ASSERT(scoredMatchIt != matchesScores.end(), "querying",
						"ScoredIndexMatches not found; is this a programming error?");

					ScoredIndexMatches &scoredMatches = scoredMatchIt.value();

					// Push this match
					scoredMatches.matches.insert(efMatch);

					// Add the score for this part to the element score


					float queryPartScoreForElement = 0;

					// Foreach term retrieve the ief and add it to the current
					// score of this element.
					// At the end of everything, this leads to a tf.ief measure
					// since the score is increased of ief for each term
					// (the "tf" part).
					foreach (QString queryPartTerm, queryPartTerms) {
						float ief = mIrModel->termScore(queryPartTerm);
						dd4("[e" << efMatch.elementSerial << "] ief(" << queryPartTerm << ") = " << ief );
						queryPartScoreForElement += ief;
					}

					// Moreover, I would like to enhance the phrasal query, for this reason
					// the score of a phrasal is the sum of the if.ief of its tokens plus a bonus
					// defined as C^(len(tokens)), so that if the phrase contain more tokens
					// the bonus increase

					float phraseBonus = pow(
						mIrModel->bonusFactorPerPhraseTerm(),
						static_cast<float>(queryPartTerms.size() - 1) // -1 for avoid to bonus non phrasals
					);

					if (phraseBonus > 1) {
						vv2("Giving bonus of " << phraseBonus
						   << " to phrase: " << queryPartTerms.join(" "));
						queryPartScoreForElement *= phraseBonus;
					}

#if DEBUG
					QString s = DEC(efMatch.elementSerial) + " += " +
								FLT(queryPartScoreForElement);
#endif
					// Add the score to the this scoredMatches (for a single element)
					scoredMatches.score += queryPartScoreForElement;
#if DEBUG
					s += FLT(scoredMatches.score);

					dd3(s);
#endif
				}
			}
		};

		elementScoreCalculator(partPubMatches, scoredPubs);
		elementScoreCalculator(partVenueMatches, scoredVenues);

		PROF_END(resolveQueryPartScoreComputing)

		PROF_END(resolveQueryPart)
	}


	// Print the matches of scoredPubs and scoredVenues

	auto printPartialMatches = [this](
			QHash<elem_serial, ScoredIndexMatches> &partMatches, QString matchName) {
		for (auto it = partMatches.begin(); it != partMatches.end(); it++) {
			const elem_serial serial = it.key();
			QString identifier;
			if (mIrModel->index()->identifier(serial, identifier)) {
				vv("Found raw " << matchName << " match (element = " << serial <<
				   " (" << identifier << "); score = " << it.value().score << ")");
			} else {
				ww("Could not find identifier for " << matchName << " match: " << serial);
			}
		}
	};

	printPartialMatches(scoredPubs, "publication");
	printPartialMatches(scoredVenues, "venue");


	// Privilege publication for which the crossrefed venue contains matches

	// For each publication in scoredPubs, check if a venue in scoredVenues
	// exists (following the crossref), if so push a pub+venue with an enhanced
	// score (at least the sum of the two).
	// Otherwise push the publication as it is, with its score.
	// After that, check the venue in scoredVenues that have not been pushed
	// and push those as they are.

	// Keep the venues that has already been crosreferred in order to not
	// insert those between the matches
	QSet<elem_serial> crossReferredVenues;

	PROF_BEGIN(crossrefsCheck)

	for (auto it = scoredPubs.begin(); it != scoredPubs.end(); it++) {

		const elem_serial pubSerial = it.key();
		const ScoredIndexMatches &scoredPubMatches = it.value();

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

				const ScoredIndexMatches &scoredVenueMatches = *crossrefVenueIt;

				// Compute the score (at least the sum)

				dd3("[" << pubSerial << "] score = " << scoredPubMatches.score);
				dd3("[" << crossrefSerial << "] score = " << scoredVenueMatches.score);
				dd3("Bonus factor = " << mIrModel->bonusFactorForPublicationVenueMatch());

				float enhancedScore =
						(scoredPubMatches.score + scoredVenueMatches.score) *
						mIrModel->bonusFactorForPublicationVenueMatch();

				vv2("Pushing pub+venue match for [" << pubSerial << "]" <<
					" => [" << crossrefSerial << "] - score = " << enhancedScore);

				matches.append(QueryMatch::forPublicationVenue(
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

			matches.append(QueryMatch::forPublication(scoredPubMatches.matches, score));
		}
	}

	// VENUE matches (the venue not crossreferred yet)

	for (auto it = scoredVenues.begin(); it != scoredVenues.end(); it++) {
		const elem_serial venueSerial = it.key();

		if (crossReferredVenues.contains(venueSerial)) {
			dd1("Venue [" << venueSerial << "] is already part of a pub+venue; ignoring it");
			continue;
		}

		const ScoredIndexMatches &scoredVenueMatches = it.value();

		// Venue do not appear in any pub+venue, push it

		float score = scoredVenueMatches.score *
				mIrModel->bonusFactorForVenueMatch();

		vv1("Pushing venue match for [" << venueSerial << "] - score = " << score);
		matches.append(QueryMatch::forVenue(scoredVenueMatches.matches, score));
	}
	PROF_END(crossrefsCheck)

	PROF_BEGIN(sortQueryMatches)

	// Finally, sort the query matches by score (reverse)
	std::sort(matches.rbegin(), matches.rend());

	PROF_END(sortQueryMatches)

	PROF_FUNC_END

	return matches;
}
