#include "gui_query_match.h"

GuiQueryMatch::GuiQueryMatch() {}

GuiQueryMatch::GuiQueryMatch(const QueryMatch &match)
{
	mModel = match;
}

QueryMatch GuiQueryMatch::model() const
{
	return mModel;
}
