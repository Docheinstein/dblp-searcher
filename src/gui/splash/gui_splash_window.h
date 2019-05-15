#ifndef GUI_SPLASH_WINDOW_H
#define GUI_SPLASH_WINDOW_H

#define GUI_SPLASH_WINDOW_QML_NAME "SplashWindow"

#include <QObject>
#include "gui/base/component/gui_component.h"

class QQmlEngine;
class QJSEngine;

class GuiSplashWindow : public GuiComponent {

	Q_OBJECT

	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(double progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
	static QObject * qmlInstance(QQmlEngine *qeng, QJSEngine *jseng);
	static GuiSplashWindow & instance();

	QString status();
	void setStatus(const QString &status);

	double progress();
	void setProgress(double progress);

signals:
	void statusChanged();
	void progressChanged();

private:
	QString qmlName() override;

	QString mStatus = "";

	double mLastNotifiedProgress = 0;
	double mProgress = 0;

};

#endif // GUI_SPLASH_WINDOW_H
