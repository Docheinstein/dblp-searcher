#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include <QObject>
#include <QString>

#include "commons/log/loggable/loggable.h"

class QQmlComponent;

class GuiComponent : public QObject, protected Loggable {
	Q_OBJECT

	Q_PROPERTY(bool shown READ shown WRITE setShown NOTIFY shownChanged)

public:
	virtual ~GuiComponent() override;

	bool shown() const;
	void setShown(bool shown);

	bool create();

protected:
	LOGGING_OVERRIDE

	virtual QString qmlName() = 0;
	void createComponent();

	QQmlComponent *mComponent;

	bool mShown = false;

signals:
	void shownChanged();
	void componentCreated();
};

#endif // GUI_COMPONENT_H
