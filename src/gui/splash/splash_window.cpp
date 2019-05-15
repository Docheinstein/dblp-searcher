#include "splash_window.h"

QObject *SplashWindow::qmlInstance(QQmlEngine *qeng, QJSEngine *jseng)
{
	Q_UNUSED(qeng)
	Q_UNUSED(jseng)
	return &instance();
}

SplashWindow &SplashWindow::instance()
{
	static SplashWindow INSTANCE;
	return INSTANCE;
}

QString SplashWindow::status()
{
	return mStatus;
}

void SplashWindow::setStatus(const QString &status)
{
	mStatus = status;
	emit statusChanged();
}

double SplashWindow::progress()
{
	return mProgress;
}

void SplashWindow::setProgress(double progress)
{
	mProgress = progress;
	emit progressChanged();
}

QString SplashWindow::qmlName()
{
	return SPLASH_WINDOW_QML_NAME;
}

QString SplashWindow::contextName()
{
	return SPLASH_WINDOW_CONTEXT_NAME;
}
