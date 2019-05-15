#ifndef GUI_QUERY_RESOLVER_H
#define GUI_QUERY_RESOLVER_H

#define GUI_QUERY_RESOLVER_QML_NAME "QueryResolver"

#include <QObject>
#include <QVector>
#include <QFutureWatcher>
#include "dblp/query/match/query_match.h"
#include "gui/base/component/gui_component.h"
#include "gui/models/query_matches/gui_query_matches.h"

class QQmlEngine;
class QJSEngine;

class QueryResolver;

class GuiQueryResolver : public QObject, Loggable {

	Q_OBJECT

	Q_PROPERTY(Status status READ status NOTIFY statusChanged)

	Q_PROPERTY(int queryTime READ queryTime NOTIFY queryTimeChanged)
	Q_PROPERTY(int matchesCount READ matchesCount NOTIFY matchesCountChanged)

	Q_PROPERTY(QObject * matches READ matches NOTIFY matchesChanged)

public:
	virtual ~GuiQueryResolver() override;
	GuiQueryResolver();

	enum class Status {
		None,
		Resolving,
		Done,
	};
	Q_ENUM(Status)

	static QObject * qmlInstance(QQmlEngine *qeng, QJSEngine *jseng);
	static GuiQueryResolver & instance();
	
	void setResolver(QueryResolver *resolver);
	QueryResolver * resolver();

	// Properties
	Status status();
	int queryTime();
	int matchesCount();
	QObject * matches();


public slots:
	void doSearch(const QString &query);
	void searchStarted();
	void searchFinished();

signals:
	void statusChanged();
	void queryTimeChanged();
	void matchesChanged();
	void matchesCountChanged();

protected:
	LOGGING_OVERRIDE

private:
	static Logger L;

	QVector<QueryMatch> doSearchReal(const QString &queryString);
	void setStatus(Status status);
	void setQueryTime(int ms);

	QueryResolver *mResolver;
	QFutureWatcher<QVector<QueryMatch>> mQueryWatcher;
	
	Status mStatus = Status::None;
	int mQueryTime = 0;
	GuiQueryMatches mMatches;
};


#endif // GUI_QUERY_RESOLVER_H
