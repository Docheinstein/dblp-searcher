#include "gui_splash_window.h"

#define PROGRESS_NOTIFICATION_THRESHOLD 0.01

QObject *GuiSplashWindow::qmlInstance(QQmlEngine *qeng, QJSEngine *jseng)
{
	Q_UNUSED(qeng)
	Q_UNUSED(jseng)
	return &instance();
}

GuiSplashWindow &GuiSplashWindow::instance()
{
	static GuiSplashWindow INSTANCE;
	return INSTANCE;
}

QString GuiSplashWindow::qmlName()
{
	return GUI_SPLASH_WINDOW_QML_NAME;
}

QString GuiSplashWindow::status()
{
	return mStatus;
}

void GuiSplashWindow::setStatus(const QString &status)
{
	mStatus = status;
	emit statusChanged();
}

double GuiSplashWindow::progress()
{
	return mProgress;
}

void GuiSplashWindow::setProgress(double progress)
{
	if (progress - mLastNotifiedProgress > PROGRESS_NOTIFICATION_THRESHOLD) {
		// Do not notify too many times
		mLastNotifiedProgress = progress;
		emit progressChanged();
	}

	mProgress = progress;
}

