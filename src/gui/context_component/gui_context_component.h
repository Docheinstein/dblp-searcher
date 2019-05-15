#ifndef GUI_CONTEXT_COMPONENT_H
#define GUI_CONTEXT_COMPONENT_H

#include "gui/component/gui_component.h"

class QQmlComponent;

class GuiContextComponent : public GuiComponent {

	Q_OBJECT

public:
	bool create();

protected:
	QQmlComponent *mComponent;

	virtual QString qmlName() = 0;
	virtual QString contextName() = 0;

	void createComponent();

signals:

	void componentCreated();
};

#endif // GUI_CONTEXT_COMPONENT_H
