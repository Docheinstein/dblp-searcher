#ifndef SPLASH_WINDOW_H
#define SPLASH_WINDOW_H

#define SPLASH_WINDOW_QML_NAME "SplashWindow"
#define SPLASH_WINDOW_CONTEXT_NAME "_splash"

#include <QObject>
#include "gui/context_component/gui_context_component.h"

class QQmlEngine;
class QJSEngine;

class SplashWindow : public GuiContextComponent {

	Q_OBJECT

	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(double progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
	static QObject * qmlInstance(QQmlEngine *qeng, QJSEngine *jseng);
	static SplashWindow & instance();

	QString status();
	void setStatus(const QString &status);

	double progress();
	void setProgress(double progress);

signals:
	void statusChanged();
	void progressChanged();

protected:
	QString qmlName() override;
	QString contextName() override;

	QString mStatus = "";
	double mProgress = 0;

};

#endif // SPLASH_WINDOW_H
