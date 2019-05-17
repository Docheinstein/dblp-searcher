#ifndef DBLP_XML_PARSER_H
#define DBLP_XML_PARSER_H

#include <QFile>
#include <QXmlStreamReader>
#include "commons/log/loggable/loggable.h"
#include "dblp/xml/handler/dblp_xml_parse_handler.h"

class DblpXmlParser : protected Loggable
{
public:
	DblpXmlParser(const QString &inputFilePath, DblpXmlParseHandler &parseHandler);
	bool parse();

protected:
	LOGGING_OVERRIDE

private:
	QFile mInput;
	DblpXmlParseHandler &mHandler;
	QXmlStreamReader mReader;

	QString mElementName;
	QXmlStreamAttributes mElementAttributes;
	QMap<QString, QVector<QString>> mElementFields;

	int mLastNotifiedPercentage = 0;

	void handleStartElement();
	void handleStartField(const QString &field);
	void handleEndElement(bool resetState = true);

	void resetParserState();

};

#endif // DBLP_XML_PARSER_H
