#include "main_window.h"

#include "dblp/query/resolver/query_resolver.h"
#include "dblp/query/match/query_match.h"
#include "commons/globals/globals.h"
#include <QThread>
#include <QtConcurrentRun>

LOGGING(MainWindow, true)

#define STATUS_NONE "none"
#define STATUS_RESOLVING "resolving"
#define STATUS_RESOLVED "resolved"

QString MainWindow::qmlName() { return "_main"; }
QString MainWindow::qmlResourceName() { return "main"; }

MainWindow::MainWindow()
{
	setStatus(STATUS_NONE);
}

void MainWindow::setResolver(QueryResolver *resolver)
{
	mResolver = resolver;
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

void MainWindow::doSearch(const QString &query)
{
	ii("Performing search for: " << query << "");
	setStatus(STATUS_RESOLVING);

	QtConcurrent::run(this, &MainWindow::doSearchReal, query);
}

void MainWindow::doSearchReal(const QString &query)
{
	QList<QueryMatch> matches = mResolver->resolveQuery(query);

	for (auto it = matches.begin(); it != matches.end(); it++) {
		const QueryMatch &queryMatch = *it;

		if (queryMatch.matchType() == QueryMatchType::Publication) {
			ii("Found PUBLICATION match: " << queryMatch.publication() <<
			   "; score = " << queryMatch.score());
		}
		else if (queryMatch.matchType() == QueryMatchType::Venue) {
			ii("Found VENUE match: " << queryMatch.venue() <<
			   "; score = " << queryMatch.score());
		}
		else if (queryMatch.matchType() == QueryMatchType::PublicationVenue) {
			ii("Found PUB+VENUE match: " <<
			   queryMatch.publication() << " => " << queryMatch.venue() <<
			   "; score = " << queryMatch.score());
		}
	}

	ii("Search done; # results = " << matches.size());
	setStatus(STATUS_RESOLVED);
}

