#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QFutureWatcher>
#include <gui/ui/window/base/gui_window.h>
#include <dblp/query/match/query_match.h>
#include "gui/models/query_matches/gui_query_matches.h"

class QueryResolver;
class GuiQueryMatch;

class MainWindow : public GuiWindow {
	Q_OBJECT

	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(int queryTime READ queryTime WRITE setQueryTime NOTIFY queryTimeChanged)
	Q_PROPERTY(QObject * matches READ matches NOTIFY matchesChanged)
	Q_PROPERTY(int matchesCount READ matchesCount NOTIFY matchesCountChanged)

public:
	MainWindow();
	enum class MainWindowStatus {
		None,
		QueryResolving,
		QueryDone
	};
	Q_ENUM(MainWindowStatus)

	void setResolver(QueryResolver *resolver);
	QueryResolver * resolver();

	QString status();
	void setStatus(QString status);

	QObject * matches();

	int queryTime();
	void setQueryTime(int ms);

	int matchesCount();

public slots:
	void doSearch(const QString &query);
	void searchStarted();
	void searchFinished();

signals:
	void statusChanged();
	void queryTimeChanged();
	void matchesChanged();
	void matchesCountChanged();

private:
	static Logger L;
	QString qmlName() override;
	QString qmlResourceName() override;

	QFutureWatcher<QVector<QueryMatch>> mQueryWatcher;

	QueryResolver *mResolver;

	QString mStatus;
	GuiQueryMatches mMatches;
	int mQueryTime;

	const char *logTag() const override;
	bool canLog() const override;

	QVector<QueryMatch> doSearchReal(const QString &query);
};

#endif // MAIN_WINDOW_H
