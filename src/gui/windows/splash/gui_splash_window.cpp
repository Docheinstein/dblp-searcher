#include "gui_splash_window.h"
#include <QThread>

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

QString GuiSplashWindow::status() const
{
	return mStatus;
}

void GuiSplashWindow::setStatus(const QString &status)
{
	mStatus = status;
	emit statusChanged();
}

double GuiSplashWindow::progress() const
{
	return mProgress;
}

void GuiSplashWindow::setProgress(double progress)
{
	if (
		// Probably a loading of a different stuff started; allowed
		progress < mProgress

			||

		// If the progress is gone further, notify only if the new progress
		// differs from the previous by at least a certain threshold
		progress - mLastNotifiedProgress > PROGRESS_NOTIFICATION_THRESHOLD
	) {
		mLastNotifiedProgress = progress;
		emit progressChanged();
	}
	// else: do not notify now, but take into account anyway

	mProgress = progress;

}

