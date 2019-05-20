#ifndef DBLP_XML_PARSE_HANDLER_H
#define DBLP_XML_PARSE_HANDLER_H

#include <QtGlobal>
#include "dblp/xml/models/element/dblp_xml_element.h"

// Handler for the elements parsed by the DblpXmlParser

class DblpXmlParseHandler {
public:
	virtual ~DblpXmlParseHandler();

	virtual void onParseStart() = 0;
	virtual void onParseEnd() = 0;

	virtual bool onElement(const DblpXmlElement &element, qint64 pos) = 0;
};

#endif // DBLP_XML_PARSE_HANDLER_H
