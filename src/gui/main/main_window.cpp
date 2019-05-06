#include "main_window.h"

#include "dblp/query/resolver/query_resolver.h"

LOGGING(MainWindow, true)

QString MainWindow::qmlName() { return "_main"; }
QString MainWindow::qmlResourceName() { return "main"; }

MainWindow::MainWindow(QQmlEngine *engine) : GuiWindow (engine)
{
}

void MainWindow::setResolver(QueryResolver *resolver)
{
	mResolver = resolver;
}

QueryResolver *MainWindow::resolver()
{
	return mResolver;
}

void MainWindow::doSearch(const QString &query)
{
	dd("Performing search for '" << query << "'");
	mResolver->resolveQuery(query);
}


