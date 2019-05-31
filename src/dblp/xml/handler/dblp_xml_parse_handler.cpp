#include "dblp_xml_parse_handler.h"

DblpXmlParseHandler::~DblpXmlParseHandler() {}

void DblpXmlParseHandler::onFileOpened(const QFile &file)
{
	Q_UNUSED(file)
	// no-op
}
