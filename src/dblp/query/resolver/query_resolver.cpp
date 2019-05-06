#include "query_resolver.h"
#include "math.h"
#include <dblp/index/handler/index_handler.h>
#include "commons/globals/globals.h"
#include "commons/const/const.h"
#include "commons/config/config.h"

LOGGING(QueryResolver, true);

// --- Hashing purpose

inline bool operator==(const QueryMatch &qm1, const QueryMatch &qm2)
{
	return qm1.elementId == qm2.elementId;
}

inline uint qHash(const QueryMatch &qm, uint seed)
{
	return qHash(qm.elementId, seed);
}

// ---


QueryResolver::QueryResolver(IRModel *irmodel)
{
	mIrModel = irmodel;
}

QSet<QueryMatch> QueryResolver::resolveQuery(const Query &query)
{
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

	// Match of a query part, for which the score (~tf.ief) has been computed
	struct ScoredElementFieldMatch {
		quint32 elementId;
		float score;
	};

	QHash<quint32, float> scoredPubs;
	QHash<quint32, float> scoredVenues;

	QSet<QueryMatch> matches;

	IndexHandler *index = mIrModel->index();

	dd("Going to resolve query: " << endl << query);
	dd("Query parts # = " << query.parts().size());

//	QSet<ElementFieldMatch> pubMatches;
//	QSet<ElementFieldMatch> venueMatches;

	// Resolve each query part (the query part are ORred)
	foreach (QueryBasePart *part, query.parts()) {
		dd1("Resolving part of query: " << *part);
		// For each bunch of tokens of the part (which might be either
		// a simple word or phrase) we have to perform findElements();

		QHash<QString, QSet<ElementFieldMatch>> partPubMatches;
		QHash<QString, QSet<ElementFieldMatch>> partVenueMatches;

		foreach (QString macroToken, part->tokens()) {
			dd2("Resolving subpart of query for token: " << macroToken);

			// Figure out if we are treating publications or venue (or both, for All)
			ElementFieldTypes searchFields = part->elementFieldTypes();
			int searchFieldsFlags = static_cast<int>(searchFields);

			// Check if the flags belong to publication types or venue types
			// We have to do this double direction check for tollerate both
			// a superset or a subset of the flags
			bool isPublication =
					searchFields.testFlag((ElementFieldType::Publication)) ||
					((searchFieldsFlags &
					 static_cast<int>(ElementFieldType::Publication)) == searchFieldsFlags);
			bool isVenue =
					searchFields.testFlag((ElementFieldType::Venue)) ||
					((searchFieldsFlags &
					 static_cast<int>(ElementFieldType::Venue)) == searchFieldsFlags);



			// Potentially we could treat the case All for global searches,
			// thus perform two calls to findElements()

			if (!isPublication && !isVenue) {
				ee("The field type flags (" << searchFieldsFlags <<
				   ") do not belong neither to publication or venue, WTF?");

				ee("Search flags     : " << searchFields << " ("
				   << BIN(searchFieldsFlags) << ")");
				ee("Publication flags: " << searchFields << " ("
				   << BIN(static_cast<int>(ElementFieldType::Publication)) << ")");
				ee("Venue flags      : " << searchFields << " ("
				   << BIN(static_cast<int>(ElementFieldType::Venue)) << ")");
				exit(-1);
			}

			if (isPublication) {
				vv("-- treating search (" << macroToken << ") for PUBLICATION type");

				// Take just the publications; thus put searchFields in AND
				// with the Publication flags (this is needed for ensure
				// that the All type takes just the publications)
				// Obviously we cannot use just ElementFieldType::Publication,
				// otherwise we would throw away any other filter of the query part
				QSet<ElementFieldMatch> partPubMatchesSet;

				index->findElements(macroToken,
									searchFields & ElementFieldType::Publication,
									partPubMatchesSet);

				partPubMatches.insert(macroToken, partPubMatchesSet);
			}

			if (isVenue) {
				vv("-- treating search (" << macroToken << ") for VENUE type");

				// Take just the venues; thus put searchFields in AND
				// with the Venue flags (this is needed for ensure
				// that the All type takes just the venues)
				// Obviously we cannot use just ElementFieldType::Venue,
				// otherwise we would throw away any other filter of the query part
				QSet<ElementFieldMatch> partVenueMatchesSet;

				index->findElements(macroToken,
									searchFields & ElementFieldType::Venue,
									partVenueMatchesSet);

				partVenueMatches.insert(macroToken, partVenueMatchesSet);

			}
		}

		// Compute the score for each retrieved element (pub or venue)
		// and add it to the current score of that element

		vv1("computing query part score...");

		// First of all, split the macro token into simple terms for which
		// we know the ief


		auto elementScoreCalculator = [this](
				QHash<QString, QSet<ElementFieldMatch>> &partMatches,
				QHash<quint32, float> &scores) {

			for (auto it = partMatches.begin(); it != partMatches.end(); it++) {
				const QString &macroToken = it.key();
				const QSet<ElementFieldMatch> &matches = it.value();

				QStringList queryPartTerms =
						macroToken.split(Const::Dblp::Query::TOKENS_SPLITTER,
						QString::SplitBehavior::SkipEmptyParts);

				foreach (ElementFieldMatch efMatch, matches) {
					float currentElementScore = scores.value(efMatch.elementId, 0);
					float queryPartScore = 0;

					foreach (QString queryPartTerm, queryPartTerms) {
						// The if.ief of a query part is defined equal to the sum of the if.ief of
						// its tokens.
						// For each publication/venue retrieved, compute tf.ief of a every
						// single term and add it to the score
						// The match count is actually the tf, but extended to
						// the cases in which the match is a phrase and not a single term
						float ief = mIrModel->termScore(queryPartTerm);
						vv3("[e" << efMatch.elementId << "] ief(" << queryPartTerm << ") = " << ief );
						vv3("[e" << efMatch.elementId << "] #match = " << efMatch.matchCount);

						float tfief = ief *	efMatch.matchCount;
						vv4("[e" << efMatch.elementId << "] tfief(" << queryPartTerm << ") = " << tfief );

						queryPartScore += tfief;

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
						queryPartScore *= phraseBonus;
					}

					// Push the new score for the element
					scores.insert(efMatch.elementId, currentElementScore + queryPartScore);
				}
			}
		};

		elementScoreCalculator(partPubMatches, scoredPubs);
		elementScoreCalculator(partVenueMatches, scoredVenues);
	}

	// Obtain the matches from the scoredPubs and scoredVenues

	for (auto it = scoredPubs.begin(); it != scoredPubs.end(); it++) {
		ii("Publication match (element = " << it.key() << "; score = " << it.value());
		matches.insert({it.key()});
	}
	for (auto it = scoredVenues.begin(); it != scoredVenues.end(); it++) {
		ii("Venue match (element = " << it.key() << "; score = " << it.value());
		matches.insert({it.key()});
	}

	return matches;
}
