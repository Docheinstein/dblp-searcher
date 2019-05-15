#include "gui_element_details.h"
#include "commons/globals/globals.h"

LOGGING(GuiElementDetails, true)

int GuiElementDetails::elementSerial()
{
	return INT(mElementSerial);
}

void GuiElementDetails::setElementSerial(int serial)
{
	dd("Called GuiElementDetails.setElementSerial() for serial: " << serial);
	mElementSerial = static_cast<elem_serial>(serial);
	emit elementSerialChanged();
}

QString GuiElementDetails::elementXml()
{
	return mElementXml;
}

void GuiElementDetails::setElementXml(const QString &xml)
{
	mElementXml = xml;
	emit elementXmlChanged();
}
