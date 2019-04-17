#ifndef DBLP_XML_PARSER_H
#define DBLP_XML_PARSER_H

#include <QString>
#include <QXmlContentHandler>
#include <QList>
#include "commons/logger/logger.h"
#include "dblp/entities/dblp_entities.h"

class DblpXmlParseHandler;

class DblpXmlParser : QXmlDefaultHandler
{
public:
	DblpXmlParser(QString &dblpXmlPath,
				  DblpXmlParseHandler &parseHandler);
	bool parse();

private:
	Logger L = Logger::forClass("Parser");

	// Index path
//	QString mInputPath;

	QFile mInputFile;
	qint64 mInputFileSize;

	// Handler that will be notified about parsed elements
	DblpXmlParseHandler *mHandler;

	// Last tag content
	QString mLastText;

	// Last notified percentage (not functional)
	int mLastNotifiedPercentage = 0;

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
	void setDocumentLocator(QXmlLocator*);
	bool startDocument();
	bool endDocument();
	bool startElement(const QString&,
					  const QString& localName,
					  const QString&,
					  const QXmlAttributes& atts);
	bool endElement(const QString&,
					const QString& localName,
					const QString&);
	bool characters(const QString& chars);
	bool ignorableWhitespace(const QString& ch) override;
	bool processingInstruction(const QString& target, const QString& data) override;
	bool skippedEntity(const QString& name) override;

   // XML SAX error callbacks
   bool error(const QXmlParseException& exception);
   bool fatalError(const QXmlParseException& exception);
   bool warning(const QXmlParseException& exception);

   QString errorString() const;

   void handleParseError();
};

#endif // DBLP_XML_PARSER_H
