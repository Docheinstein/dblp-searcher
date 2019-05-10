#ifndef DBLP_PARSER_H
#define DBLP_PARSER_H

#include <QString>
#include <QXmlContentHandler>
#include <QList>
#include <QElapsedTimer>
#include <commons/log/loggable/loggable.h>
#include "dblp/xml/models/entities/xml_entities.h"

class XmlParseHandler;

class XmlParser : public QXmlDefaultHandler, protected Loggable
{
public:
	XmlParser(const QString &dblpXmlPath,
				  XmlParseHandler &parseHandler);
	bool parse();

protected:
	LOGGING_OVERRIDE

private:
	// Index path
//	QString mInputPath;

	QFile mInputFile;
	qint64 mInputFileSize;

	// Handler that will be notified about parsed elements
	XmlParseHandler *mHandler;

	// Last tag content
	QString mLastText;

	// Last notified percentage (not functional)
	int mLastNotifiedPercentage = 0;

	// Parsing time (not functional)
	QElapsedTimer mParseTimer;

	// Current model of parsed element and fields
	QString mKey;
	QList<QString> mAuthors;
	QString mTitle;
	QString mYear;
	QString mJournal;
	QString mPublisher;
	QString mBooktitle;
	QString mCrossref;

	void resetFields();

	// XML SAX content callbacks
	bool startDocument() override;
	bool endDocument() override;
	bool startElement(const QString&, const QString&localName,
					  const QString&,
					  const QXmlAttributes& atts) override;
	bool endElement(const QString&,
					const QString& localName,
					const QString&) override;
	bool characters(const QString& chars) override;
	bool skippedEntity(const QString& name) override;

	// bool ignorableWhitespace(const QString& ch) override;
	// bool processingInstruction(const QString& target, const QString& data) override;

   // XML SAX error callbacks
   bool error(const QXmlParseException& exception) override;
   bool fatalError(const QXmlParseException& exception) override;
   bool warning(const QXmlParseException& exception) override;

   QString errorString() const override;

   void handleParseError();
};

#endif // DBLP_PARSER_H