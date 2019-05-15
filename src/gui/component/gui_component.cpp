#include "gui_component.h"

LOGGING(GuiComponent, true)

GuiComponent::~GuiComponent()
{
}

bool GuiComponent::shown()
{
	return mShown;
}

void GuiComponent::setShown(bool shown)
{
	mShown = shown;
	emit shownChanged();
}

