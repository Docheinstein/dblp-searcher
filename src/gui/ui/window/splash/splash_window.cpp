#include "splash_window.h"

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


QString SplashWindow::qmlName() { return "_splash"; }
QString SplashWindow::qmlResourceName() { return "splash"; }
