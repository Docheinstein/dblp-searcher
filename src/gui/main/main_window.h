#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gui/window/gui_window.h>

class MainWindow : public GuiWindow {
	Q_OBJECT

public:
	MainWindow(QQmlEngine *engine);

public slots:
	void doSearch(const QString &query);

protected:
	QString qmlName();
	QString qmlResourceName();
};

#endif // MAIN_WINDOW_H
