#include "query_resolver.h"
#include "math.h"
#include <dblp/index/handler/index_handler.h>

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

Logger QueryResolver::L = Logger::forClass("QueryResolver");

QueryResolver::QueryResolver(IRModel *irmodel)
{
	mIrModel = irmodel;
}

QSet<QueryMatch> QueryResolver::resolveQuery(const Query &query)
{
	QSet<QueryMatch> matches;

	IndexHandler *index = mIrModel->index();

	dd("Going to resolve query: " << endl << query);
	dd("Query parts # = " << query.parts().size());

	QSet<ElementFieldMatch> efMatches;

	foreach (QueryBasePart *part, query.parts()) {
		dd("Resolving part of query: " << *part);
		// For each bunch of tokens of the part (which might be either
		// a simple word or phrase) we have to perform findElements();
		foreach (QString token, part->tokens()) {
			dd("- Resolving subpart of query for token: " << token);
			index->findElements(token,
								part->elementFieldTypes(),
								efMatches);
		}
	}

	dd("Found total of " << efMatches.size() << " matches ");

	foreach (ElementFieldMatch match, efMatches) {
		dd("- EFmatch id: " << match.elementId);
	}

	return matches;
}
