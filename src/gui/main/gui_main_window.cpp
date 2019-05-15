#include "gui_main_window.h"

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
