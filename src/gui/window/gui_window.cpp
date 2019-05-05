#include "gui_window.h"

GuiWindow::GuiWindow(QQmlEngine *engine) : GuiComponent (engine)
{

}

bool GuiWindow::visible()
{
	return mVisible;
}

void GuiWindow::setVisible(bool visible)
{
	mVisible = visible;
	emit visibleChanged();
}
