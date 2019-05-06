#include "query.h"
#include <commons/const/const.h>
#include <commons/util/util.h>
#include "commons/globals/globals.h"
#include <dblp/query/models/publication/query_publication_part.h>
#include <dblp/query/models/basic/query_basic_part.h>
#include <dblp/query/models/venue/query_venue_part.h>

namespace QueryConst = Const::Dblp::Query;
namespace QueryUtil = Util::Dblp::Query;

LOGGING(Query, true)

const QList<QueryBasePart *> Query::parts() const
{
	return mQueryParts;
}

Query::Query(const QString &queryString)
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

	// Just wrap a string remind if it was a phrasal or not
	typedef struct QueryToken {
		QString token;
		bool phrasal;
	} QueryToken;

	QList<QueryToken> queryTokens;

	// Retrieve the macro tokens of the query, by splitting by double quotes
	QStringList macroTokens = queryString.split(
				Const::Dblp::Query::PHRASAL_MARK,
				QString::SplitBehavior::KeepEmptyParts);

	// At this point we should have inside macroTokens some pharasal as tokens.
	// We should have alternatively one non phrasal token and one token
	// e.g. There "is a cat" inside "my table"
	//		|_____||_______||_______||________|
	//		  simp	  phra    simp      phra
	// Note that in this way we handle also the case in which the last closing double
	// quote is not present; everything after the last double quote would
	// be considered a token.
	// For handle the case in which there is only one token, we have to keep
	// the empty parts and skip those later, but we have to keep those to figure
	// out if the position is even/odd


	int i = 0;
	for (auto it = macroTokens.begin(); it != macroTokens.end(); it++, i++) {
		const QString &macroToken = *it;
		vv1("Found macro token: " << macroToken);
		// Skip empty tokens (may occur for single token queries)
		if (macroToken.isEmpty()) {
			vv2("Skipping since empty");
		}
		// Not empty, check if it's phrasal or simple
		else if (i % 2 == 1) {
			vv2("Adding as phrasal");
			// even macro tokens are phrasals, add those as they are
			queryTokens.append({macroToken, true /* phrasal */});
		} else {
			vv2("Splitting by ' ' since not phrasal");
			// odds macro tokens are not phrasal, split those by spaces
			// and adds every part as a single token
			QStringList macroTokenTokens =
					macroToken.split(Const::Dblp::Query::TOKENS_SPLITTER,
							  QString::SplitBehavior::SkipEmptyParts);

			foreach (QString macroTokenToken, macroTokenTokens) {
				vv3("Adding as simple: " << macroTokenToken);
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
	auto addTokenToQueryPart = [this](QueryBasePart *&part, const QString &token) {
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
		 * 2b) In case we have a search pattern but the token has not been
		 *		readed until the end, push the remaining string as a token
		 *		for the new query part.
		 *		e.g. phThesis:Harald => we have to push Harald to the phThesis part
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
		// The new query part may already contain a token
		// (this happens if, for example, the search pattern was attached to
		// a token of that searc pattern, e.g. phThesis:Harald)

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
				addPart(currentQueryPart);

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
		addPart(currentQueryPart);
	}

	dd("Query has been parsed:" << endl << *this);
}

// If the token is a search pattern, returns true and put into part
// the new query part
QueryBasePart * Query::newQueryPartFromToken(const QString &token)
{
	QueryBasePart * queryPart = nullptr;

	if ((queryPart = newQueryPublicationPartFromToken(token)) != nullptr)
		return queryPart;

	if ((queryPart = newQueryVenuePartFromToken(token)) != nullptr)
		return queryPart;

	return nullptr; // not a search pattern
}

QueryBasePart * Query::newQueryPublicationPartFromToken(const QString &token)
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

	return nullptr;
}

QueryBasePart * Query::newQueryPublicationPartFromToken(
		const QString &token,
		QueryPublicationElementCase pubElement)
{
	// At this point we can have
	// 0 nope) Nothing that starts with "<publication>*"
	// 1 ok) "<publication>:" => Generic search pattern without field filter
	// 2 ok) "<publication>." => Search patten with field filter
	// 3 nope) "<publication><something_else>" => Not a search pattern but just
	// tokens of the previous search pattern

	QueryBasePart * part = nullptr;

	// 1) Search pattern without a field (<publication>:) ?

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement, nullptr)))
		return part;

	// 2) Search pattern with a field (<publication>.<field>*) ?

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement,
			 QueryPublicationFieldType::TITLE)))
		return part;

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement,
			 QueryPublicationFieldType::YEAR)))
		return part;

	if ((part = newQueryPublicationPartFromToken(
			 token, pubElement,
			 QueryPublicationFieldType::AUTHOR)))
		return part;

	// 3) Nope, not a known search pattern

	return nullptr;
}

QueryBasePart *Query::newQueryPublicationPartFromToken(
		const QString &token,
		QueryPublicationElementCase pubElement,
		QueryPublicationFieldCase pubField)
{
	return newQueryPartFromToken(token, pubElement, pubField,
			[](QueryElementType *el, QueryFieldType *fl) -> QueryBasePart *{
		return new QueryPublicationPart(
					static_cast<QueryPublicationElementCase>(el),
					static_cast<QueryPublicationFieldCase>(fl)
		);
	});
}

QueryBasePart *Query::newQueryVenuePartFromToken(const QString &token)
{
	// The only allowed venue element is VENUE
	return newQueryVenuePartFromToken(token, QueryVenueElementType::VENUE);
}

QueryBasePart *Query::newQueryVenuePartFromToken(
		const QString &token,
		QueryVenueElementCase venueElement)
{
	// At this point we can have
	// 0 nope) Nothing that starts with "<venue>*"
	// 1 ok) "venue:" => Generic search pattern without field filter
	// 2 ok) "venue." => Search patten with field filter
	// 3 nope) "venue<something_else>" => Not a search pattern but just
	// tokens of the previous search pattern

	QueryBasePart * part = nullptr;

	// 1) Search pattern without a field (<publication>:) ?

	if ((part = newQueryVenuePartFromToken(
			 token, venueElement, nullptr)))
		return part;

	// 2) Search pattern with a field (<publication>.<field>*) ?

	if ((part = newQueryVenuePartFromToken(
			 token, venueElement,
			 QueryVenueFieldType::TITLE)))
		return part;

	if ((part = newQueryVenuePartFromToken(
			 token, venueElement,
			 QueryVenueFieldType::PUBLISHER)))
		return part;

	// 3) Nope, not a known search pattern

	return nullptr;
}

QueryBasePart *Query::newQueryVenuePartFromToken(
		const QString &token,
		QueryVenueElementCase venueElement,
		QueryVenueFieldCase venueField)
{
	return newQueryPartFromToken(token, venueElement, venueField,
			[](QueryElementType *el, QueryFieldType *fl) -> QueryBasePart *{
		return new QueryVenuePart(
					static_cast<QueryVenueElementCase>(el),
					static_cast<QueryVenueFieldCase>(fl)
		);
	});
}

QueryBasePart *Query::newQueryPartFromToken(
		const QString &token,
		QueryElementType *element,
		QueryFieldType *field,
		QueryBasePart *queryPartCreator(QueryElementType *, QueryFieldType *))
{
	QString searchPattern = QueryUtil::queryPartSearch(
				element->string(),
				field ? field->string() : "" /* no field */);

	if (!token.startsWith(searchPattern)) {
		// Search pattern doesn't match
		// Not a search pattern of type
		// "<publication>.<field>:" or "<publication/field>:"
		// It might be that
		// 1) The token doesn't contain the search pattern
		// 2) The token doesn't start with the search pattern
		return nullptr;
	}

	// At this point, it means that the token is a search pattern
	// (it begins with "<publication>.<field>:" or "<publication/field>:")
	// We have to capture eventual other tokens that occure after the search pattern

	QString remainingToken = token.mid(searchPattern.size());

	// FIXME: eventually we could check the remaining token recursively

	// <publication>.<field>:
	QueryBasePart *part = queryPartCreator(element, field);

	// Add the remaining token part if it is valid
	if (!remainingToken.isEmpty()) {
		_vv("Adding remaining token: " << remainingToken);
		part->addToken(remainingToken);
	}

	return part;
}

Query::~Query()
{
	// Dealloc query parts
	qDeleteAll(mQueryParts);
}

void Query::addPart(QueryBasePart * queryPart)
{
	mQueryParts.append(queryPart);
}

Query::operator QString() const
{
	QString s;
	int i = 0;
	for (auto it = mQueryParts.begin(); it != mQueryParts.end();) {
		s += DEC(i) + ") " + **it;

		it++;
		i++;

		if (it != mQueryParts.end())
			 s += "\n";
	}

	return s;
}
