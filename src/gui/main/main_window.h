#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <gui/window/gui_window.h>

class QueryResolver;

class MainWindow : public GuiWindow {
	Q_OBJECT

	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)

public:
	MainWindow();
//	enum class MainWindowStatus {
//		None,
//		QueryResolving,
//		QueryDone
//	};
//	Q_ENUM(MainWindowStatus)

	void setResolver(QueryResolver *resolver);
	QueryResolver * resolver();

	QString status();
	void setStatus(QString status);

public slots:
	void doSearch(const QString &query);

signals:
	void statusChanged();

protected:
	static Logger L;
	QString qmlName() override;
	QString qmlResourceName() override;

	QueryResolver *mResolver;

	QString mStatus;

	const char *logTag() const override;
	bool canLog() const override;

	void doSearchReal(const QString &query);
};

#endif // MAIN_WINDOW_H
