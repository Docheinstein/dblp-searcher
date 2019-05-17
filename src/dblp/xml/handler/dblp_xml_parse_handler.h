#ifndef DBLP_XML_PARSE_HANDLER_H
#define DBLP_XML_PARSE_HANDLER_H

#include <QtGlobal>
#include "dblp/xml/models/element/dblp_xml_element.h"

class DblpXmlParseHandler {
public:
	virtual ~DblpXmlParseHandler();

	virtual void onParseStart() = 0;
	virtual void onParseEnd() = 0;

	virtual void onElement(const DblpXmlElement &element, qint64 pos) = 0;
};

#endif // DBLP_XML_PARSE_HANDLER_H
