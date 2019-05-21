#include "gui_element_publication.h"

#include "dblp/index/handler/index_handler.h"
#include "dblp/query/resolver/query_resolver.h"
#include "dblp/irmodel/base/ir_model.h"
#include "gui/main/gui_main_window.h"


elem_serial GuiElementPublication::serial() const
{
	return mSerial;
}

QString GuiElementPublication::identifier() const
{
	return mIdentifier;
}

GuiElementPublication::Builder &GuiElementPublication::Builder::serial(elem_serial serial)
{
	mSerial = serial;
	return *this;
}

GuiElementPublication GuiElementPublication::Builder::build()
{
	GuiElementPublication pub;
	pub.mSerial = mSerial;

	GuiMainWindow::instance().resolver()->irModel()->index()
			.identifier(mSerial, pub.mIdentifier);

	return pub;
}
