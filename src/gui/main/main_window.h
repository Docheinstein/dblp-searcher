#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gui/window/gui_window.h>

class QueryResolver;

class MainWindow : public GuiWindow {
	Q_OBJECT

public:
	MainWindow(QQmlEngine *engine);

	void setResolver(QueryResolver *resolver);
	QueryResolver * resolver();

public slots:
	void doSearch(const QString &query);

protected:
	static Logger L;
	QString qmlName() override;
	QString qmlResourceName() override;

	QueryResolver *mResolver;

	const char *logTag() const override;
	bool canLog() const override;
};

#endif // MAIN_WINDOW_H
