#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "gui/ui/base/gui_component.h"

class GuiWindow : public GuiComponent {
	Q_OBJECT

	Q_PROPERTY(bool visible READ visible WRITE setVisible NOTIFY visibleChanged)

public:
	bool visible();
	void setVisible(bool visible);

signals:
	void visibleChanged();

protected:
	bool mVisible = false;
};


#endif // GUI_WINDOW_H
