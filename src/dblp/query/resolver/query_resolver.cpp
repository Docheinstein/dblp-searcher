#include "query_resolver.h"

#include <math.h>
#include <omp.h>

#include "commons/util/util.h"
#include "commons/globals/globals.h"
#include "commons/const/const.h"
#include "commons/config/config.h"
#include "commons/profiler/profiler.h"
#include "dblp/index/handler/index_handler.h"
#include "dblp/query/query/models/base/query_base_part.h"
#include "dblp/query/query/query.h"
#include "dblp/irmodel/base/ir_model.h"

STATIC_LOGGING(QueryResolver, true);
LOGGING(QueryResolver, true);

QueryResolver::QueryResolver() {}

QueryResolver::QueryResolver(IRModel *irmodel)
{
	mIrModel = irmodel;
}

IRModel *QueryResolver::irModel()
{
	return mIrModel;
}

struct ScoredIndexElementMatches {
	QVector<IndexMatch> matches; // the matches should have the same serial
	float score;
};

// ========== OMP REDUCTION MAP STUFF ===========

struct CrossrefsCheckReductionContainer {
	QSet<elem_serial> crossrefVenues;
	QVector<QueryMatch> matches;
	QHash<elem_serial, QVector<IndexMatch>> rawMatchesBySerial;
};

struct VenuesPushReductionContainer {
	QVector<QueryMatch> matches;
	QHash<elem_serial, QVector<IndexMatch>> rawMatchesBySerial;
};

static void computeElementsScoreCombiner(
		QHash<elem_serial, ScoredIndexElementMatches> &out,
		const QHash<elem_serial, ScoredIndexElementMatches> &in) {
	for (auto in_it = in.cbegin(); in_it != in.cend(); ++in_it) {
		auto out_it = out.find(in_it.key());
		if (out_it == out.cend()) {
			_dd4("Combiner: inserting [e" << in_it.key() <<
				"] for the first time w/ score = " << in_it.value().score);
			out.insert(in_it.key(), in_it.value());
		}
		else {
			const ScoredIndexElementMatches &in_siem = in_it.value();
			ScoredIndexElementMatches &out_siem = out_it.value();
			out_siem.score += in_siem.score;
			out_siem.matches.append(in_siem.matches);
			_dd4("Combiner: increased score of [e" << in_it.key() << "] of " <<
				 in_siem.score << ", now = " << out_siem.score);
		}
	}
}

static void checkCrossrefsCombiner(
		CrossrefsCheckReductionContainer &out,
		const CrossrefsCheckReductionContainer &in) {
	_dd4("Combiner: matches was " << out.matches.size() << ", will be "
		 << out.matches.size() + in.matches.size());
	out.matches.append(in.matches);
	out.crossrefVenues.unite(in.crossrefVenues);
	out.rawMatchesBySerial.unite(in.rawMatchesBySerial);
}

static void venuesPushCombiner(
		VenuesPushReductionContainer &out,
		const VenuesPushReductionContainer &in) {
	_dd4("Combiner: matches was " << out.matches.size() << ", will be "
		 << out.matches.size() + in.matches.size());
	out.matches.append(in.matches);
	out.rawMatchesBySerial.unite(in.rawMatchesBySerial);
}

// ========== OMP REDUCTION MAP STUFF END ===========

QueryOutcome QueryResolver::resolveQuery(const Query &query) {
	PROF_FUNC_BEGIN1

	IndexHandler &index = mIrModel->index();

	QueryOutcome outcome;

	QHash<elem_serial, ScoredIndexElementMatches> scoredPubs;
	QHash<elem_serial, ScoredIndexElementMatches> scoredVenues;

	QVector<IndexMatch> publicationMatches;
	QVector<IndexMatch> venueMatches;

	PROF_BEGIN2(resolveQueryFind)

	// Find matches (posts)

	for (QueryBasePart *part : query.parts()) {

		vv1("Resolving part of query: " << *part);

		for (const QString &macroToken : part->tokens()) {

			// If lazy ief is required, compute all the needed ief now (since its
			// not thread safe to compute in omp for, and use a lock would be an overkill.
			// Those will be necessary for compute the elements score

#if LAZY_IEF
			QStringList tokens = macroToken.split(
						Const::Dblp::Query::TOKENS_SPLITTER,
						QString::SplitBehavior::SkipEmptyParts);
			for (auto it = tokens.cbegin(); it < tokens.cend(); ++it) {
#if VERBOSE
				// Compute & print
				float ief = mIrModel->termScore(Util::String::sanitizeTerm(*it));
				vv4("Computing ief(" << *it << ") now due lazy ief: " << ief);
#else
				// Just compute
				mIrModel->termScore(Util::String::sanitizeTerm(*it));
#endif // VERBOSE
			}
#endif // LAZY_IEF

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

				index.findMatches(macroToken,
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

				index.findMatches(macroToken,
								searchFields & ElementFieldType::Venue,
								venueMatches);
			}
		}
	}

	PROF_END(resolveQueryFind)

	// Compute score of elements among the matches

	PROF_BEGIN2(resolveQueryScoreComputing)

	#pragma omp declare reduction( \
		computeElementsScoreReducer : \
		QHash<elem_serial, ScoredIndexElementMatches> : \
			computeElementsScoreCombiner(omp_out, omp_in))

	#pragma omp parallel
	{
		auto computeElementsScore = [&](const QVector<IndexMatch> matches,
					QHash<elem_serial, ScoredIndexElementMatches> &scoredMatches) {

			// Compute publications and venues scores
			#pragma omp for nowait reduction(computeElementsScoreReducer:scoredMatches)
			for (int i = 0; i < matches.size(); ++i) {
				const IndexMatch &match = matches.at(i);

				// Retrieve the current score for this element, if it doesn't exist
				// insert a new one

				if (!scoredMatches.contains(match.elementSerial)) {
					dd4("Creating ScoredIndexMatches for element = " << match.elementSerial);
					// With the no match (for now, than this efMatch will
					// be pushed) and score = 0 (which will
					// be increased later)
					scoredMatches.insert(match.elementSerial, {{}, 0});
				}

				auto scoredMatchIt = scoredMatches.find(match.elementSerial);

				ASSERT(scoredMatchIt != scoredMatches.end(), "querying",
					"ScoredIndexMatches not found; is this a programming error?");

				ScoredIndexElementMatches &scoredElementMatches = scoredMatchIt.value();
				float matchPartialScore = 0;

				// Push this match
				scoredElementMatches.matches.append(match);

				// For each matched token inside this match, retrieve the ief
				// and add it to the global score of the associated element
				for (const QString &token : match.matchedTokens) {
					float ief;
					ief = mIrModel->termScore(token);
					dd4("[e" << match.elementSerial << "] ief(" << token << ") = " << ief );
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
					   << " to phrase: " << match.matchedTokens.join(" "));
					matchPartialScore *= phraseBonus;
				}

				// Add the score to the this scoredMatches (for a single element)
				scoredElementMatches.score += matchPartialScore;

				dd3("[e" << DEC(match.elementSerial) + "] += " + FLT(matchPartialScore));
				dd4("    = " + FLT(scoredElementMatches.score));
			}
		};

		computeElementsScore(publicationMatches, scoredPubs); // Invoked with nowait
		computeElementsScore(venueMatches, scoredVenues);	  // Invoked with nowait

		// Implicit barrier
	}

	PROF_END(resolveQueryScoreComputing)

	// Check whether the the publications have a crossref among the matches
	// and push those as pub+venue eventually

	PROF_BEGIN2(crossrefsCheck)

	CrossrefsCheckReductionContainer crossrefCheckReductionContainer;

	// Try to guess the space to reserve, in order to prevent reallocation.
	// If we had no pub+venue than the count would be exactly
	// scoredPubs.size() + scoredVenues.size(),
	// Since the pub+venue is not so common to have, that is our guess anyway
	crossrefCheckReductionContainer.matches.reserve(
				scoredPubs.size() + scoredVenues.size());
	crossrefCheckReductionContainer.rawMatchesBySerial.reserve(
				scoredPubs.size() + scoredVenues.size());

	const QList<elem_serial> &scoredPubsSerials = scoredPubs.keys();
	const QList<ScoredIndexElementMatches> &scoredPubsElements = scoredPubs.values();

	PROF_BEGIN3(crossrefsCheckPubs)

	#pragma omp declare reduction( \
		checkCrossrefsReducer : \
		CrossrefsCheckReductionContainer : \
			checkCrossrefsCombiner(omp_out, omp_in))

	#pragma omp parallel for reduction(checkCrossrefsReducer:crossrefCheckReductionContainer)
	for (int i = 0; i < scoredPubsElements.size(); ++i) {

		elem_serial scoredPubSerial = scoredPubsSerials.at(i);
		const ScoredIndexElementMatches &scoredPub = scoredPubsElements.at(i);

		// Push to the raw outcome's index matches
		crossrefCheckReductionContainer.rawMatchesBySerial.insert(
					scoredPubSerial, scoredPub.matches);

		// PUB+VENUE match

		// We have to meet two condition for declare a pub+venue, match
		// first of all the crossref must exists
		// within the index, furthermore we should have the venue between our
		// venue matches to consider this a pub+venue match.
		bool crossrefMatch = false;
		elem_serial crossrefSerial;

		vv1("Checking crossref existence for element = " << scoredPubSerial);

		// Retrieve the crossref
		if (index.crossref(scoredPubSerial, crossrefSerial)) {
			// Crossref found, check if we have crossrefSerial among
			// our matches venues

			dd2("Crossref found; " << scoredPubSerial << " => " << crossrefSerial);

			const auto &crossrefVenueIt = scoredVenues.constFind(crossrefSerial);

			if (crossrefVenueIt != scoredVenues.cend()) {

				// We have both a publication and the crossreffed venue among
				// our pub matches and venue matches, enhance the score
				// and push a pub+venue match
				dd3("Crossref found and venue is among matched; "
					"this is actually a pub+venue match");

				crossrefMatch = true; // do not push the publication
									  // by itself later, but now as a pub+venue now

				crossrefCheckReductionContainer.crossrefVenues.insert(crossrefSerial);

				const ScoredIndexElementMatches &scoredVenueMatches = *crossrefVenueIt;

				// Compute the score (at least the sum)

				dd3("[" << scoredPubSerial << "] score = " << scoredPub.score);
				dd3("[" << crossrefSerial << "] score = " << scoredVenueMatches.score);
				dd3("Bonus factor = " << mIrModel->bonusFactorForPublicationVenueMatch());

				float enhancedScore =
						(scoredPub.score + scoredVenueMatches.score) *
						mIrModel->bonusFactorForPublicationVenueMatch();

				vv2("Pushing pub+venue match for [" << scoredPubSerial << "]" <<
					" => [" << crossrefSerial << "] - score = " << enhancedScore);

				crossrefCheckReductionContainer.matches.append(
							QueryMatch::forPublicationVenue(
							   scoredPub.matches, scoredVenueMatches.matches,
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

			float score = scoredPub.score *
					mIrModel->bonusFactorForPublicationMatch();

			vv2("Pushing pub match for [" << scoredPubSerial << "] - score = " << score);

			crossrefCheckReductionContainer.matches.append(
						QueryMatch::forPublication(
							scoredPub.matches, score));
		}
	}

	PROF_END(crossrefsCheckPubs)

	dd1("Pubs count # = " << scoredPubs.size());
	dd1("Venues count # = " << scoredVenues.size());
	dd1("Crossrefed venues count # = " << crossrefCheckReductionContainer.matches.size());
	dd1("Crossrefed venues count # = " << crossrefCheckReductionContainer.crossrefVenues.size());

	PROF_BEGIN3(crossrefsPushVenues)

	// And now push the remaining venues as venue only

	VenuesPushReductionContainer venuesReductionContainer;


	// Try to guess the space to reserve, in order to prevent reallocation.
	// If we had no pub+venue than the count would be exactly
	// scoredPubs.size() + scoredVenues.size(),
	// Since the pub+venue is not so common to have, that is our guess anyway
	venuesReductionContainer.matches.reserve(scoredVenues.size());
	venuesReductionContainer.rawMatchesBySerial.reserve(scoredVenues.size());

	// VENUE matches (the venue not crossreferred yet)
	const QList<elem_serial> &scoredVenuesSerials = scoredVenues.keys();
	const QList<ScoredIndexElementMatches> &scoredVenuesElements = scoredVenues.values();

	#pragma omp declare reduction( \
		insertVenueInQueryMatchReducer : \
		VenuesPushReductionContainer : \
			venuesPushCombiner(omp_out, omp_in))

	#pragma omp parallel for reduction(insertVenueInQueryMatchReducer:venuesReductionContainer)
	for (int i = 0; i < scoredVenuesSerials.size(); ++i) {

		const elem_serial venueSerial = scoredVenuesSerials.at(i);
		const ScoredIndexElementMatches &scoredVenueMatches = scoredVenuesElements.at(i);

		// Push to the raw outcome's index matches
		venuesReductionContainer.rawMatchesBySerial
				.insert(venueSerial, scoredVenueMatches.matches);

		if (crossrefCheckReductionContainer.crossrefVenues.contains(venueSerial)) {
			dd1("Venue [" << venueSerial << "] is already part of a pub+venue; ignoring it");
			continue;
		}

		// Venue do not appear in any pub+venue, push it

		float score = scoredVenueMatches.score * mIrModel->bonusFactorForVenueMatch();

		vv1("Pushing venue match for [" << venueSerial << "] - score = " << score);
		venuesReductionContainer.matches.append(QueryMatch::forVenue(
						scoredVenueMatches.matches, score));
	}

	PROF_END(crossrefsPushVenues)

	PROF_END(crossrefsCheck)

	PROF_BEGIN2(sortQueryMatches)

	// Push the matches from the two steps to the outcome
	outcome.sortedQueryMatches.append(venuesReductionContainer.matches);
	outcome.sortedQueryMatches.append(crossrefCheckReductionContainer.matches);
	outcome.indexMatchesBySerial.unite(venuesReductionContainer.rawMatchesBySerial);
	outcome.indexMatchesBySerial.unite(crossrefCheckReductionContainer.rawMatchesBySerial);

	// Finally, sort the query matches by score (reverse)
	std::sort(outcome.sortedQueryMatches.rbegin(), outcome.sortedQueryMatches.rend());

	PROF_END(sortQueryMatches)

	PROF_FUNC_END

	// Print those for debug
#if VERBOSE
	int i = 0;
	for (const QueryMatch &match : outcome.sortedQueryMatches) {
		vv(i << "° MATCH === " << endl << match);
		++i;
	}
#endif

	return outcome;
}
