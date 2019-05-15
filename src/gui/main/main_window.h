#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#define MAIN_WINDOW_QML_NAME "SplashWindow"
#define MAIN_WINDOW_CONTEXT_NAME "_splash"

#include <QObject>
#include "gui/context_component/gui_context_component.h"

class MainWindow : public GuiContextComponent {

//	Q_PROPERTY(QString status READ status WRITE setStatus NOTIFY statusChanged)
//	Q_PROPERTY(double progress READ progress WRITE setProgress NOTIFY progressChanged)

//public:

//	QString status();
//	void setStatus(const QString &status);

//	double progress();
//	void setProgress(double progress);

//signals:
//	void statusChanged();
//	void progressChanged();

//protected:
//	QString qmlName() override;
//	QString contextName() override;

//	QString mStatus = "";
//	double mProgress = 0;
};

#endif // MAIN_WINDOW_H
