#include "gui_main_window.h"

#include <QElapsedTimer>
#include <QThread>
#include <QtConcurrentRun>
#include <QFuture>
#include <QFutureWatcher>

#include "dblp/query/resolver/query_resolver.h"
#include "dblp/query/match/query_match.h"
#include "dblp/query/resolver/query_resolver.h"
#include "dblp/query/query/query.h"
#include "commons/globals/globals.h"
#include "commons/profiler/profiler.h"
#include "gui/components/query_match/gui_query_match.h"

LOGGING(GuiMainWindow, false)

GuiMainWindow::~GuiMainWindow() {}

GuiMainWindow::GuiMainWindow() {
	// Connect the future end to the searchFinished slot
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

QueryResolver *GuiMainWindow::resolver() const
{
	return mResolver;
}

// PROPERTIES

GuiMainWindow::QueryStatus GuiMainWindow::queryStatus() const
{
	return mQueryStatus;
}


QObject *GuiMainWindow::matches()
{
	return &mMatches;
}

const QVector<IndexMatch> GuiMainWindow::elementMatches(elem_serial serial) const
{
	return mMatchesHash.value(serial, {});
}

int GuiMainWindow::matchesCount()
{
	return mMatches.size();
}


int GuiMainWindow::queryTime() const
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

#if EASTER_EGGS
	// Easter egg for profiling

	if (query == QLatin1String("profreset")) {
		profReset();
		return;
	}

	if (query == QLatin1String("profprint")) {
		profPrint();
		return;
	}
#endif // EASTER_EGGS

	ii("Performing search for: " << query << "");

	emit searchStarted();

	QFuture<QueryOutcome> queryFuture =
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


QueryOutcome GuiMainWindow::doSearchReal(const QString &queryString)
{

	PROF_FUNC_BEGIN0

	QElapsedTimer queryTimer;
	queryTimer.start();

	Query query(queryString);
	QueryOutcome outcome = mResolver->resolveQuery(query);

#if VERBOSE
	for (auto it = outcome.sortedQueryMatches.cbegin(); it != outcome.sortedQueryMatches.cend(); ++it) {
		const QueryMatch &queryMatch = *it;

		if (queryMatch.matchType() == QueryMatchType::Publication) {
			vv("Found PUBLICATION match: " << queryMatch.publication() <<
			   "; score = " << FLT(queryMatch.score()));
		}
		else if (queryMatch.matchType() == QueryMatchType::Venue) {
			vv("Found VENUE match: " << queryMatch.venue() <<
			   "; score = " << FLT(queryMatch.score()));
		}
		else if (queryMatch.matchType() == QueryMatchType::PublicationVenue) {
			vv("Found PUB+VENUE match: " <<
			   queryMatch.publication() << " => " << queryMatch.venue() <<
			   "; score = " << FLT(queryMatch.score()));
		}
	}
#endif


	ii("Query took: " << queryTimer.elapsed() << "ms");

	setQueryTime(INT(queryTimer.elapsed()));

	PROF_FUNC_END

	return outcome;
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
	mMatchesHash.clear();
}

void GuiMainWindow::searchFinished()
{
	const QueryOutcome &outcome = mQueryWatcher.result();

	ii("Search done; # results = " << outcome.sortedQueryMatches.size());

#if PROFILER
	profPrint();
#endif

	mMatches.addMatches(outcome.sortedQueryMatches);
	mMatchesHash = outcome.indexMatchesBySerial;

	emit matchesCountChanged();
	setQueryStatus(QueryStatus::Done);
}
