#ifndef GUI_MAIN_WINDOW_H
#define GUI_MAIN_WINDOW_H

#define GUI_MAIN_WINDOW_QML_NAME "MainWindow"

#include <QObject>
#include <QVector>
#include <QHash>
#include <QFutureWatcher>
#include <dblp/query/resolver/query_resolver.h>
#include "dblp/query/match/query_match.h"
#include "gui/base/component/gui_component.h"
#include "gui/models/query_matches/gui_query_matches.h"

class QQmlEngine;
class QJSEngine;

class QueryResolver;

class GuiMainWindow : public GuiComponent {

	Q_OBJECT

	Q_PROPERTY(QueryStatus queryStatus READ queryStatus NOTIFY queryStatusChanged)

	Q_PROPERTY(int queryTime READ queryTime NOTIFY queryTimeChanged)
	Q_PROPERTY(int matchesCount READ matchesCount NOTIFY matchesCountChanged)

	Q_PROPERTY(QObject * matches READ matches NOTIFY matchesChanged)

public:
	virtual ~GuiMainWindow() override;
	GuiMainWindow();

	enum class QueryStatus {
		None,
		Resolving,
		Done,
	};
	Q_ENUM(QueryStatus)

	static QObject * qmlInstance(QQmlEngine *qeng, QJSEngine *jseng);
	static GuiMainWindow & instance();

	void setResolver(QueryResolver *resolver);
	QueryResolver * resolver();

	// Properties
	QueryStatus queryStatus();
	int queryTime();
	int matchesCount();
	QObject * matches();

	QVector<IndexMatch> elementMatches(elem_serial serial);


public slots:
	void doSearch(const QString &query);
	void openElementDetails(int serial);

	void searchStarted();
	void searchFinished();

	void popView();

protected:
	LOGGING_OVERRIDE
	QString qmlName() override;

signals:
	void queryStatusChanged();
	void queryTimeChanged();
	void matchesChanged();
	void matchesCountChanged();

	void openElementDetailsRequired(int serial);
	void popViewRequired();

private:
	static Logger L;

	QueryOutcome doSearchReal(const QString &queryString);
	void setQueryStatus(QueryStatus status);
	void setQueryTime(int ms);

	QueryResolver *mResolver;
	QFutureWatcher<QueryOutcome> mQueryWatcher;

	QueryStatus mQueryStatus = QueryStatus::None;
	int mQueryTime = 0;
	GuiQueryMatches mMatches;
	QHash<elem_serial, QVector<IndexMatch>> mMatchesHash;
};

#endif // GUI_MAIN_WINDOW_H
