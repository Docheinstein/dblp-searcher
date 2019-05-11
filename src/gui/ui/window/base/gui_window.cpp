#include "gui_window.h"

bool GuiWindow::visible()
{
	return mVisible;
}

void GuiWindow::setVisible(bool visible)
{
	mVisible = visible;
	emit visibleChanged();
}
