#ifndef GUI_COMPONENT_H
#define GUI_COMPONENT_H

#include <QObject>
#include <QString>
#include <commons/log/loggable/loggable.h>

class GuiComponent : public QObject, Loggable {
	Q_OBJECT

	Q_PROPERTY(bool shown READ shown WRITE setShown NOTIFY shownChanged)

public:
	virtual ~GuiComponent() override;

	bool shown();
	void setShown(bool shown);

protected:
	LOGGING_OVERRIDE

	static Logger L;

	bool mShown = false;

signals:
	void shownChanged();
};

#endif // GUI_COMPONENT_H
