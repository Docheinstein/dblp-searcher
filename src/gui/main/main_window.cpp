#include "main_window.h"

MainWindow::MainWindow(QQmlEngine *engine) : GuiWindow (engine)
{

}

void MainWindow::doSearch(const QString &query)
{
	dd("Performing search for '" << query << "'");
}

QString MainWindow::qmlName() { return "_main"; }
QString MainWindow::qmlResourceName() { return "main"; }
