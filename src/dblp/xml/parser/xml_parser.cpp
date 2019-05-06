#include "xml_parser.h"

#include <QXmlSimpleReader>
#include "dblp/xml/handler/xml_parse_handler.h"
#include "commons/const/const.h"
#include "commons/util/util.h"
#include "dblp/xml/models/types/xml_types.h"

#define VERBOSE_XML 0

namespace DblpXml = Const::Dblp::Xml;

LOGGING(XmlParser, true)

XmlParser::XmlParser(const QString &dblpXmlPath,
					XmlParseHandler &parseHandler)
{
//	mInputPath = QString(dblpXmlPath);
	mInputFile.setFileName(dblpXmlPath);
	mHandler = &parseHandler;
}

bool XmlParser::parse()
{
	ii("Parsing DBLP file at " << mInputFile.fileName());

	QXmlSimpleReader xmlParser;

	xmlParser.setContentHandler(this);
	xmlParser.setErrorHandler(this);

	mInputFile.open(QFile::ReadOnly);
	mInputFileSize = mInputFile.size();
	QXmlInputSource xmlSource(&mInputFile);

	mParseTimer.start();
	xmlParser.parse(&xmlSource);

	return true;
}

void XmlParser::resetFields() {
	mAuthors.clear();
	mTitle = "";
	mYear = "";
	mPublisher = "";
	mBooktitle = "";
	mCrossref = "";
}

void XmlParser::setDocumentLocator(QXmlLocator *)
{

}

bool XmlParser::startDocument()
{
	ii("Started XML parsing...");

	mHandler->onStart();
	return true;
}

bool XmlParser::endDocument()
{
	ii("Finished XML parsing");
	ii("Parse took " <<
	   Util::Time::humanTime(static_cast<int>(mParseTimer.elapsed())));
	mHandler->onEnd();
	return true;
}

bool XmlParser::startElement(const QString&,
							   const QString& localName,
							   const QString&,
							   const QXmlAttributes& atts)
{
#if VERBOSE_XML
	vv("<" << localName << ">");
	vv("	" << atts.length() << " atts");
	vv("	 pos: " << mInputFile.pos());
#endif

	// For root elements, reset the fields
	int keyIndex = atts.index(QString(XmlAttributeType::KEY->string()));
	if (keyIndex >= 0) {
		// This element should be a valid root element
		mKey = atts.value(keyIndex);

		resetFields();
	}

	// No matters what, the text now is empty
	mLastText = "";

	// Notify progress

	int perc = static_cast<int>((static_cast<double>(mInputFile.pos()) / mInputFileSize) * 100);

	if (perc > mLastNotifiedPercentage) {
		mLastNotifiedPercentage = perc;
		ii("Parsing progress: " << mLastNotifiedPercentage << "%");
	}

	return true;
}

bool XmlParser::endElement(const QString&,
							 const QString& localName,
							 const QString&)
{
#if VERBOSE_XML
	vv("</" << localName << ">");
#endif

	// Notify the element to the handler if the tag belong to and element
	// Otherwise push the last text as field of the current model

	if (localName == *XmlElementType::ARTICLE) {
		DblpArticle article;
		article.key = mKey;
		article.authors = mAuthors;
		article.title = mTitle;
		article.year = mYear;
		mHandler->onArticle(article, mInputFile.pos());
	}
	else if (localName == *XmlElementType::INCOLLECTION) {
		DblpIncollection incollection;
		incollection.key = mKey;
		incollection.authors = mAuthors;
		incollection.title = mTitle;
		incollection.year = mYear;
		incollection.booktitle = mBooktitle;
//		incollection.crossref = mCrossref;
		mHandler->onIncollection(incollection, mInputFile.pos());
	}
	else if (localName == *XmlElementType::BOOK) {
		DblpBook book;
		book.key = mKey;
		book.authors = mAuthors;
		book.title = mTitle;
		book.year = mYear;
		book.publisher = mPublisher;
		mHandler->onBook(book, mInputFile.pos());
	}
	else if (localName == *XmlElementType::INPROCEEDINGS) {
		DblpInproceedings inproc;
		inproc.key = mKey;
		inproc.authors = mAuthors;
		inproc.title = mTitle;
		inproc.year = mYear;
		inproc.booktitle = mBooktitle;
//		inproc.crossref = mCrossref;
		mHandler->onInproceedings(inproc, mInputFile.pos());
	}
	else if (localName == *XmlElementType::PROCEEDINGS) {
		DblpProceedings proc;
		proc.key = mKey;
		proc.title = mTitle;
		proc.year = mYear;
		proc.booktitle = mBooktitle;
		mHandler->onProceedings(proc, mInputFile.pos());
	}
	else if (localName == *XmlElementType::PHDTHESIS) {
		DblpPhdThesis phd;
		phd.key = mKey;
		phd.authors = mAuthors;
		phd.title = mTitle;
		phd.year = mYear;
		mHandler->onPhdThesis(phd, mInputFile.pos());
	}
	else if (localName == *XmlElementType::MASTERTHESIS) {
		DblpMasterThesis master;
		master.key = mKey;
		master.authors = mAuthors;
		master.title = mTitle;
		master.year = mYear;
		mHandler->onMasterThesis(master, mInputFile.pos());
	}

	// For the fields, put the last text into the current model

	else if (localName == *XmlFieldType::AUTHOR)
		mAuthors.append(mLastText);
	else if (localName == *XmlFieldType::TITLE)
		mTitle = mLastText;
	else if (localName == *XmlFieldType::YEAR)
		mYear = mLastText;
	else if (localName == *XmlFieldType::PUBLISHER)
		mPublisher = mLastText;
	else if (localName == *XmlFieldType::BOOKTITLE)
		mBooktitle = mLastText;
	else if (localName == *XmlFieldType::CROSSREF)
		mCrossref = mLastText;

	else if (localName == *XmlFieldType::JOURNAL) {
		// This is a special case since a journal entity doesn't exists
		// in dblm.xml; but we notify a journal only for its name on an article
		// element
		DblpJournal journal;
		journal.name = mLastText;
		mHandler->onJournal(journal, mInputFile.pos());
	}

	return true;
}

bool XmlParser::characters(const QString &chars)
{
	vv("Got chars");
#if VERBOSE_XML
	vv("\t" << chars);
#endif

	// += instead of = since text could be notified
	// in chunks instead of in a single shot
	mLastText += chars;

	return true;
}

bool XmlParser::ignorableWhitespace(const QString &ch)
{
//	vv("ignorableWhitespace: " << ch);

	return true;
}

bool XmlParser::processingInstruction(const QString &target, const QString &data)
{
//	vv("processingInstruction: " << target << " | " << data);

	return true;
}

bool XmlParser::skippedEntity(const QString &name)
{
	QString entityString = Util::Html::entityNameToString(name);
	vv("skippedEntity: " << name);
	vv("skippedEntity to qstring: " << entityString);

	mLastText += entityString;

	return true;
}

bool XmlParser::error(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

bool XmlParser::fatalError(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

bool XmlParser::warning(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

QString XmlParser::errorString() const
{
	return "PARSE ERROR";
}
