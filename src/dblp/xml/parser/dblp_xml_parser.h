#ifndef DBLP_XML_PARSER_H
#define DBLP_XML_PARSER_H

#include <QFile>
#include <QXmlStreamReader>

#include "commons/log/loggable/loggable.h"

class DblpXmlParseHandler;

class DblpXmlParser : protected Loggable
{
public:
	DblpXmlParser(const QString &inputFilePath, DblpXmlParseHandler &parseHandler,
				  qint64 startingPosition = 0, qint64 endingPosition = 0);
	bool parse();

protected:
	LOGGING_OVERRIDE

private:
	bool handleStartElement();
	bool handleStartField(const QString &field);
	bool handleEndElement(bool resetState = true);

	void notifyProgress();

	bool setParserDevice();
	bool setParserDeviceFile();
	bool setParserDeviceBuffer();
	void resetParserState();

	QFile mInput;
	qint64 mStartingPosition;
	qint64 mEndingPosition;
	DblpXmlParseHandler &mHandler;
	QXmlStreamReader mReader;

	QString mElementName;
	QXmlStreamAttributes mElementAttributes;
	QMap<QString, QVector<QString>> mElementFields;

	int mLastNotifiedPercentage = 0;
};

#endif // DBLP_XML_PARSER_H
