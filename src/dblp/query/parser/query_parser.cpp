#include "query_parser.h"
#include <commons/const/const.h>
#include <commons/util/util.h>
#include <commons/logger/logger.h>
#include <dblp/query/models/publication/query_publication_part.h>
#include <dblp/query/models/basic/query_basic_part.h>
#include <dblp/query/models/venue/query_venue_part.h>

namespace QueryConst = Const::Dblp::Query;
namespace QueryUtil = Util::Dblp::Query;

Logger QueryParser::L = Logger::forClass("QueryParser");

Query QueryParser::fromString(const QString &queryString)
{
	dd("Parsing query: '" << queryString << "'");

	/*
	 * Parse the string by scanning the tokens, then check if
	 * the tokens are known search-pattern, if so add those to the
	 * query for the successive tokens found until the next search-pattern
	 */

	/*
	 * The query tokens are taken by considering phrasals and taking those
	 * as tokens.
	 * This is done by splitting the queryString by double quotes and take
	 * the strings inside the quotes as they are, while splitting the others
	 * by spaces in order to take the single words.
	 */

	// Query to build
	Query query;

	// Just wrap a string remind if it was a phrasal or not
	typedef struct QueryToken {
		QString token;
		bool phrasal;
	} QueryToken;

	QList<QueryToken> queryTokens;

	// Retrieve the macro tokens of the query, by splitting by double quotes
	QStringList macroTokens = queryString.split(
				Const::Dblp::Query::PHRASAL_MARK,
				QString::SplitBehavior::SkipEmptyParts);

	// At this point we should have inside macroTokens some pharasal as tokens.
	// We should have alternatively one non phrasal token and one token
	// e.g. There "is a cat" inside "my table"
	//		|_____||_______||_______||________|
	//		  simp	  phra    simp      phra

	int i = 0;
	for (auto it = macroTokens.begin(); it != macroTokens.end(); it++, i++) {
		if (i % 2 == 1) {
			// even macro tokens are phrasals, add those as they are
			queryTokens.append({*it, true /* phrasal */});
		} else {
			// odds macro tokens are not phrasal, split those by spaces
			// and adds every part as a single token
			QStringList macroTokenTokens =
					it->split(Const::Dblp::Query::TOKENS_SPLITTER,
							  QString::SplitBehavior::SkipEmptyParts);

			foreach (QString macroTokenToken, macroTokenTokens) {
				queryTokens.append({macroTokenToken, false /* not phrasal */});
			}
		}
	}

	vv("Tokenized into: ");
	foreach (QueryToken token, queryTokens) {
		vv(token.token << " (" << (token.phrasal ? "phrasal" : "simple") << "): ");
	}

	// Query part the next tokens will be added to
	QueryBasePart * currentQueryPart = nullptr;

	// Assigned when a new query part is found
	QueryBasePart * newQueryPart = nullptr;

	// Safe way to add to currentQueryPart, initializing it if it's null
	auto addTokenToQueryPart = [](QueryBasePart *&part, const QString &token) {
		if (!part) {
			vv("--> initializing a new basic query part");
			part = new QueryBasicPart();
		}

		part->addToken(token);
	};

	foreach (QueryToken qtoken, queryTokens) {
		vv("Inspecting: '" << qtoken.token << "'");

		/*
		 * Use the following logic for parse the tokens:
		 * 1) If it is a phrasal, no matter what, push it the current query part
		 * 2) If it is not a phrasal, check if it is a known search pattern.
		 *	  If it is a known search pattern, then open a new query part and push
		 *    the previous one to the query
		 * 3) If it isn't a search pattern, push it as token for the current query
		 *	  part and go to the next token.
		 */

		// 1) Check for phrasal

		if (qtoken.phrasal) {
			vv("-> pushing as a phrasal token");
			addTokenToQueryPart(currentQueryPart, qtoken.token);
			continue;
		}

		// 2) Check for new search pattern

		newQueryPart = newQueryPartFromToken(qtoken.token);

		// If the token is a new query part, push the previous to the
		// query as query part, otherwise add the token to the current
		// query part

		if (newQueryPart) {
			// 2) New query part

			vv("-> beginning new query part: " << *newQueryPart);

			if (currentQueryPart /* always true actually */ &&
				// Push only if there are tokens inside it (for the initial
				// basic query part there are not tokens
				currentQueryPart->tokens().size() > 0) {
				vv("--> thus pushing previous query part: " << *currentQueryPart);

				// Push the previous query part
				query.addPart(currentQueryPart);

				// We cannot free it at this point since the query retains
				// pointers to the query part, we must free those later.
				// delete currentQueryPart;
			}
			// else: this is the first query part found, do not push the
			// previous to the query (since it doesn't exists yet)

			// Use the new query part by now
			currentQueryPart = newQueryPart;

			newQueryPart = nullptr; // actually not needed since newQueryPart..()
									// already set this to nullptr

			continue; // go to next token
		}

		// 3) We have a real basic token in our hands
		// (single word, not phrasal, not search pattern)

		addTokenToQueryPart(currentQueryPart, qtoken.token);
	}

	// Push the last query part

	if (currentQueryPart &&
		currentQueryPart->tokens().size() > 0) {
		vv("Pushing the last query part " << *currentQueryPart);
		query.addPart(currentQueryPart);
	}

	dd("Query has been parsed:" << endl << query);

	return query;
}

// If the token is a search pattern, returns true and put into part
// the new query part
QueryBasePart * QueryParser::newQueryPartFromToken(const QString &token)
{
	QueryBasePart * queryPart = nullptr;

	if ((queryPart = newQueryPublicationPartFromToken(token)) != nullptr)
		return queryPart;

	if ((queryPart = newQueryVenuePartFromToken(token)) != nullptr)
		return queryPart;

	return nullptr; // not a search pattern
}

QueryBasePart * QueryParser::newQueryPublicationPartFromToken(const QString &token)
{
	QueryBasePart * part = nullptr;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::PUBLICATION)))
		return part;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::ARTICLE)))
		return part;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::INCOLLECTION)))
		return part;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::INPROCEEDINGS)))
		return part;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::PHDTHESIS)))
		return part;

	if ((part = newQueryPublicationPartFromToken(token,
			QueryPublicationElementType::MASTERTHESIS)))
		return part;

	// ...

	return nullptr;
}

QueryBasePart * QueryParser::newQueryPublicationPartFromToken(const QString &token,
		QueryPublicationElementCase pubElement)
{
	const char *pubElementStr = pubElement->string();

	if (!token.startsWith(pubElementStr)) {
		return nullptr; // this token is not the given publication element
	}

	// At this point we can have
	// 1 ok) "<publication>:" => Generic search pattern without field filter
	// 2 ok) "<publication>." => Search patten with field filter
	// 3 nope) "<publication><something_else>" => Not a search pattern but just
	// tokens of the previous search pattern

	if (token == QueryUtil::queryPartSearch(pubElementStr)) {
		// 1) Search pattern without a field (<publication>:)
		return new QueryPublicationPart(pubElement);
	}

	if (!token.startsWith(pubElementStr + QueryConst::ELEMENT_FIELD_DIVISOR)) {
		return nullptr; // this tokens is not "<publication>:" nor "<publication>."
	}

	// 2) Probably a search pattern with a field (<publication>.<field>*)

	QueryBasePart * part = nullptr;

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement, QueryPublicationFieldType::TITLE)))
		return part;

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement, QueryPublicationFieldType::YEAR)))
		return part;

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement, QueryPublicationFieldType::AUTHOR)))
		return part;

	// 3) Nope, not a known search pattern

	return nullptr;
}

QueryBasePart *QueryParser::newQueryPublicationPartFromToken(
		const QString &token,
		QueryPublicationElementCase pubElement,
		QueryPublicationFieldCase pubField)
{
	if (token == QueryUtil::queryPartSearch(
				pubElement->string(),
				pubField->string())) {
		// <publication>.<field>:

		return new QueryPublicationPart(pubElement, pubField);
	}

	// Not a search pattern of type "<publication>.<field>:"

	return nullptr;
}

QueryBasePart *QueryParser::newQueryVenuePartFromToken(const QString &token)
{
	const char * venueStr = QueryVenueElementType::VENUE->string();
	if (!token.startsWith(venueStr)) {
		return nullptr; // this token is not venue element
	}

	// At this point we can have
	// 1 ok) "venue:" => Generic search pattern without field filter
	// 2 ok) "venue." => Search patten with field filter
	// 3 nope) "venue<something_else>" => Not a search pattern but just
	// tokens of the previous search pattern

	if (token == QueryUtil::queryPartSearch((venueStr))) {
		// 1) Search pattern without a field (venue:)
		return new QueryVenuePart(QueryVenueElementType::VENUE);
	}

	if (!token.startsWith(venueStr + QueryConst::ELEMENT_FIELD_DIVISOR)) {
		return nullptr; // this tokens is not "venue:" nor "venue."
	}

	// 2) Probably a search pattern with a field (venue.<field>*)

	QueryBasePart * part = nullptr;

	if ((part = newQueryVenuePartFromToken(
			 token, QueryVenueElementType::VENUE, QueryVenueFieldType::TITLE)))
		return part;

	if ((part = newQueryVenuePartFromToken(
			 token, QueryVenueElementType::VENUE, QueryVenueFieldType::PUBLISHER)))
		return part;

	// Not a search pattern of type "venue.<field>:"

	return nullptr;
}

QueryBasePart *QueryParser::newQueryVenuePartFromToken(
		const QString &token,
		QueryVenueElementCase venueElement,
		QueryVenueFieldCase venueField)
{
	if (token == QueryUtil::queryPartSearch(
				venueElement->string(),
				venueField->string())) {
		// venue.<field>:

		return new QueryVenuePart(venueElement, venueField);
	}

	// Not a search pattern of type "venue.<field>:"

	return nullptr;
}
