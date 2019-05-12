#include "main_window.h"

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

LOGGING(MainWindow, true)

#define STATUS_NONE "none"
#define STATUS_RESOLVING "resolving"
#define STATUS_RESOLVED "resolved"

QString MainWindow::qmlName() { return "_main"; }
QString MainWindow::qmlResourceName() { return "main"; }

MainWindow::MainWindow()
{
	setStatus(STATUS_NONE);

	QObject::connect(&mQueryWatcher,
					 SIGNAL(finished()),
					 this,
					 SLOT(searchFinished()));
}

void MainWindow::setResolver(QueryResolver *resolver)
{
	mResolver = resolver;
	mMatches.setIrModel(resolver->irModel());
}

QueryResolver *MainWindow::resolver()
{
	return mResolver;
}

QString MainWindow::status()
{
	return mStatus;
}

void MainWindow::setStatus(QString status)
{
	mStatus = status;
	emit statusChanged();
}

QObject *MainWindow::matches()
{
	return &mMatches;
}

int MainWindow::queryTime()
{
	return mQueryTime;
}

void MainWindow::setQueryTime(int ms)
{
	mQueryTime = ms;
	emit queryTimeChanged();
}

int MainWindow::matchesCount()
{
	return mMatches.size();
}

void MainWindow::doSearch(const QString &query)
{
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
			QtConcurrent::run(this, &MainWindow::doSearchReal, query);

	mQueryWatcher.setFuture(queryFuture);
}

QVector<QueryMatch> MainWindow::doSearchReal(const QString &query)
{
	PROF_FUNC_BEGIN0

	QElapsedTimer queryTimer;
	queryTimer.start();

	QVector<QueryMatch> matches = mResolver->resolveQuery(query);

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

	setQueryTime(INT(queryTimer.elapsed()));

	PROF_FUNC_END

	return matches;
}

void MainWindow::searchStarted()
{
	setStatus(STATUS_RESOLVING);

	mMatches.clearMatches();
}

void MainWindow::searchFinished()
{
	QVector<QueryMatch> matches = mQueryWatcher.result();

	ii("Search done; # results = " << matches.size());

	foreach (QueryMatch match, matches) {
		mMatches.addMatch(match);
	}

//	emit matchesChanged();
	emit matchesCountChanged();
	setStatus(STATUS_RESOLVED);
}

