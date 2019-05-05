#ifndef SPLASH_WINDOW_H
#define SPLASH_WINDOW_H

#include <gui/window/gui_window.h>

class SplashWindow : public GuiWindow {
	Q_OBJECT

	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
	Q_PROPERTY(double progress READ progress WRITE setProgress NOTIFY progressChanged)

public:
	SplashWindow(QQmlEngine *engine);

	QString status();
	void setStatus(const QString &status);

	double progress();
	void setProgress(double progress);

signals:
	void statusChanged();
	void progressChanged();

protected:
	QString qmlName();
	QString qmlResourceName();

	QString mStatus = "";
	double mProgress = 0;
};

#endif // SPLASH_WINDOW_H
