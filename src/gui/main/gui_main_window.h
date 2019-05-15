#ifndef GUI_MAIN_WINDOW_H
#define GUI_MAIN_WINDOW_H

#include "gui/base/component/gui_component.h"

#define GUI_MAIN_WINDOW_QML_NAME "MainWindow"

class QQmlEngine;
class QJSEngine;

class GuiMainWindow : public GuiComponent  {
	Q_OBJECT

public:
	static QObject * qmlInstance(QQmlEngine *qeng, QJSEngine *jseng);
	static GuiMainWindow & instance();

private:
	QString qmlName() override;
};

#endif // GUI_MAIN_WINDOW_H
