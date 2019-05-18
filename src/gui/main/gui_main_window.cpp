#include "gui_main_window.h"

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

LOGGING(GuiMainWindow, false)

GuiMainWindow::~GuiMainWindow() {}

GuiMainWindow::GuiMainWindow() {
	QObject::connect(&mQueryWatcher,
					 SIGNAL(finished()),
					 this,
					 SLOT(searchFinished()));
}

QObject *GuiMainWindow::qmlInstance(QQmlEngine *qeng, QJSEngine *jseng)
{
	Q_UNUSED(qeng)
	Q_UNUSED(jseng)
	return &instance();
}

GuiMainWindow &GuiMainWindow::instance()
{
	static GuiMainWindow INSTANCE;
	return INSTANCE;
}

QString GuiMainWindow::qmlName()
{
	return GUI_MAIN_WINDOW_QML_NAME;
}

void GuiMainWindow::setResolver(QueryResolver *resolver)
{
	mResolver = resolver;
	mMatches.setIrModel(resolver->irModel());
}

QueryResolver *GuiMainWindow::resolver()
{
	return mResolver;
}

// PROPERTIES

GuiMainWindow::QueryStatus GuiMainWindow::queryStatus()
{
	return mQueryStatus;
}


QObject *GuiMainWindow::matches()
{
	return &mMatches;
}


int GuiMainWindow::matchesCount()
{
	return mMatches.size();
}


int GuiMainWindow::queryTime()
{
	return mQueryTime;
}


void GuiMainWindow::doSearch(const QString &query)
{
	if (mQueryStatus == QueryStatus::Resolving) {
		ww("Already resolving query, please wait...");
		return;
	}

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
			QtConcurrent::run(this, &GuiMainWindow::doSearchReal, query);

	mQueryWatcher.setFuture(queryFuture);

	PROF_FUNC_END
}

void GuiMainWindow::openElementDetails(int serial)
{
	vv("Opening details for element: " << serial);
	emit openElementDetailsRequired(serial);
}

void GuiMainWindow::popView()
{
	vv("Popping view");
	emit popViewRequired();
}


QVector<QueryMatch> GuiMainWindow::doSearchReal(const QString &queryString)
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

void GuiMainWindow::setQueryStatus(QueryStatus status)
{
	mQueryStatus = status;
	emit queryStatusChanged();
}

void GuiMainWindow::setQueryTime(int ms)
{
	mQueryTime = ms;
	emit queryTimeChanged();
}

void GuiMainWindow::searchStarted()
{
	setQueryStatus(QueryStatus::Resolving);
	mMatches.clearMatches();
}

void GuiMainWindow::searchFinished()
{
	const QVector<QueryMatch> &matches = mQueryWatcher.result();

	ii("Search done; # results = " << matches.size());

	mMatches.addMatches(matches);

	emit matchesCountChanged();
	setQueryStatus(QueryStatus::Done);
}
