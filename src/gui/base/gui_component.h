#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include <QObject>
#include <QString>

#include <commons/log/loggable/loggable.h>

class QQmlEngine;
class QQmlComponent;
class QQmlContext;

class GuiComponent : public QObject, Loggable {
	Q_OBJECT

public:
	virtual ~GuiComponent();
	GuiComponent(QQmlEngine *engine);
	bool create();

protected:
	static Logger L;
	virtual QString qmlName() = 0;
	virtual QString qmlResourceName() = 0;

	void createComponent();

	QQmlEngine *mEngine;
	QQmlContext *mContext;
	QQmlComponent *mComponent;

	bool mCreated;

signals:
	void componentCreated();

protected:
	LOGGING_OVERRIDE
};

#endif // GUI_COMPONENT_H
