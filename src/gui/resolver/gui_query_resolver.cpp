#include "gui_query_resolver.h"

#include "dblp/query/resolver/query_resolver.h"
#include "dblp/query/match/query_match.h"
#include "commons/globals/globals.h"
#include "gui/models/query_match/gui_query_match.h"
#include <QThread>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>
#include <QElapsedTimer>
#include "commons/profiler/profiler.h"

LOGGING(GuiQueryResolver, true)

GuiQueryResolver::~GuiQueryResolver() {}

GuiQueryResolver::GuiQueryResolver()
{
	QObject::connect(&mQueryWatcher,
					 SIGNAL(finished()),
					 this,
					 SLOT(searchFinished()));
}

QObject *GuiQueryResolver::qmlInstance(QQmlEngine *qeng, QJSEngine *jseng)
{
	Q_UNUSED(qeng)
	Q_UNUSED(jseng)
	return &instance();
}

GuiQueryResolver &GuiQueryResolver::instance()
{
	static GuiQueryResolver INSTANCE;
	return INSTANCE;
}

void GuiQueryResolver::setResolver(QueryResolver *resolver)
{
	mResolver = resolver;
	mMatches.setIrModel(resolver->irModel());
}

QueryResolver *GuiQueryResolver::resolver()
{
	return mResolver;
}

// PROPERTIES

GuiQueryResolver::Status GuiQueryResolver::status()
{
	return mStatus;
}


QObject *GuiQueryResolver::matches()
{
	return &mMatches;
}


int GuiQueryResolver::matchesCount()
{
	return mMatches.size();
}


int GuiQueryResolver::queryTime()
{
	return mQueryTime;
}


void GuiQueryResolver::doSearch(const QString &query)
{
	PROF_FUNC_BEGIN0

	// Easter egg for profiling

	if (query == QLatin1String("r")) {
		prof_reset();
		return;
	}

	if (query == QLatin1String("p")) {
		prof_print();
		return;
	}

	ii("Performing search for: " << query << "");

	emit searchStarted();

	QFuture<QVector<QueryMatch>> queryFuture =
			QtConcurrent::run(this, &GuiQueryResolver::doSearchReal, query);

	mQueryWatcher.setFuture(queryFuture);

	PROF_FUNC_END
}

QVector<QueryMatch> GuiQueryResolver::doSearchReal(const QString &queryString)
{

	PROF_FUNC_BEGIN0

	QElapsedTimer queryTimer;
	queryTimer.start();

	Query query(queryString);
	QVector<QueryMatch> matches = mResolver->resolveQuery(query);

#if INFO
	for (auto it = matches.begin(); it != matches.end(); it++) {
		const QueryMatch &queryMatch = *it;

		if (queryMatch.matchType() == QueryMatchType::Publication) {
			ii("Found PUBLICATION match: " << queryMatch.publication() <<
			   "; score = " << FLT(queryMatch.score()));
		}
		else if (queryMatch.matchType() == QueryMatchType::Venue) {
			ii("Found VENUE match: " << queryMatch.venue() <<
			   "; score = " << FLT(queryMatch.score()));
		}
		else if (queryMatch.matchType() == QueryMatchType::PublicationVenue) {
			ii("Found PUB+VENUE match: " <<
			   queryMatch.publication() << " => " << queryMatch.venue() <<
			   "; score = " << FLT(queryMatch.score()));
		}
	}
#endif


	ii("Query took: " << queryTimer.elapsed() << "ms");

	setQueryTime(INT(queryTimer.elapsed()));

	PROF_FUNC_END

	return matches;
}

void GuiQueryResolver::setStatus(GuiQueryResolver::Status status)
{
	mStatus = status;
	emit statusChanged();
}

void GuiQueryResolver::setQueryTime(int ms)
{
	mQueryTime = ms;
	emit queryTimeChanged();
}

void GuiQueryResolver::searchStarted()
{
	setStatus(Status::Resolving);
	mMatches.clearMatches();
}

void GuiQueryResolver::searchFinished()
{
	PROF_FUNC_BEGIN

	const QVector<QueryMatch> &matches = mQueryWatcher.result();

	ii("Search done; # results = " << matches.size());

	mMatches.addMatches(matches);

	//	emit matchesChanged();
	emit matchesCountChanged();
	setStatus(Status::Done);

	PROF_FUNC_END
}
