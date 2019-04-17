#include "dblp_xml_parser.h"

#include <QXmlSimpleReader>
#include "dblp/parse/handler/dblp_xml_parse_handler.h"
#include "commons/const/const.h"
#include "commons/util/util.h"

#define VERBOSE_XML 1

DblpXmlParser::DblpXmlParser(QString &dblpXmlPath,
							 DblpXmlParseHandler &parseHandler)
{
//	mInputPath = QString(dblpXmlPath);
	mInputFile.setFileName(dblpXmlPath);
	mHandler = &parseHandler;
}

bool DblpXmlParser::parse()
{
	ii("Parsing DBLP file at " << mInputFile.fileName());

	QXmlSimpleReader xmlParser;

	xmlParser.setContentHandler(this);
	xmlParser.setErrorHandler(this);

	mInputFile.open(QFile::ReadOnly);
	mInputFileSize = mInputFile.size();
	QXmlInputSource xmlSource(&mInputFile);

	xmlParser.parse(&xmlSource);

	return true;
}

void DblpXmlParser::resetFields() {
	mAuthors.clear();
	mTitle = "";
	mYear = "";
	mPublisher = "";
	mBooktitle = "";
	mCrossref = "";
}

void DblpXmlParser::setDocumentLocator(QXmlLocator *)
{

}

bool DblpXmlParser::startDocument()
{
	ii("Started XML parsing...");

	mHandler->onStart();
	return true;
}

bool DblpXmlParser::endDocument()
{
	ii("Finished XML parsing");

	mHandler->onEnd();
	return true;
}

bool DblpXmlParser::startElement(const QString&,
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
	int keyIndex = atts.index(Const::Dblp::Attributes::KEY);
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

bool DblpXmlParser::endElement(const QString&,
							 const QString& localName,
							 const QString&)
{
#if VERBOSE_XML
	vv("</" << localName << ">");
#endif

	// Notify the element to the handler if the tag belong to and element
	// Otherwise push the last text as field of the current model

	if (localName == Const::Dblp::Elements::ARTICLE) {
		DblpArticle article;
		article.key = mKey;
		article.authors = mAuthors;
		article.title = mTitle;
		article.year = mYear;
		mHandler->onArticle(article, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::INCOLLECTION) {
		DblpIncollection incollection;
		incollection.key = mKey;
		incollection.authors = mAuthors;
		incollection.title = mTitle;
		incollection.year = mYear;
		incollection.booktitle = mBooktitle;
//		incollection.crossref = mCrossref;
		mHandler->onIncollection(incollection, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::BOOK) {
		DblpBook book;
		book.key = mKey;
		book.authors = mAuthors;
		book.title = mTitle;
		book.year = mYear;
		book.publisher = mPublisher;
		mHandler->onBook(book, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::INPROCEEDINGS) {
		DblpInproceedings inproc;
		inproc.key = mKey;
		inproc.authors = mAuthors;
		inproc.title = mTitle;
		inproc.year = mYear;
		inproc.booktitle = mBooktitle;
//		inproc.crossref = mCrossref;
		mHandler->onInproceedings(inproc, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::PROCEEDINGS) {
		DblpProceedings proc;
		proc.key = mKey;
		proc.title = mTitle;
		proc.year = mYear;
		proc.booktitle = mBooktitle;
		mHandler->onProceedings(proc, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::PHDTHESIS) {
		DblpPhdThesis phd;
		phd.key = mKey;
		phd.authors = mAuthors;
		phd.title = mTitle;
		phd.year = mYear;
		mHandler->onPhdThesis(phd, mInputFile.pos());
	}
	else if (localName == Const::Dblp::Elements::MASTERTHESIS) {
		DblpMasterThesis master;
		master.key = mKey;
		master.authors = mAuthors;
		master.title = mTitle;
		master.year = mYear;
		mHandler->onMasterThesis(master, mInputFile.pos());
	}

	// For the fields, put the last text into the current model

	else if (localName == Const::Dblp::Fields::AUTHOR)
		mAuthors.append(mLastText);
	else if (localName == Const::Dblp::Fields::TITLE)
		mTitle = mLastText;
	else if (localName == Const::Dblp::Fields::YEAR)
		mYear = mLastText;
	else if (localName == Const::Dblp::Fields::PUBLISHER)
		mPublisher = mLastText;
	else if (localName == Const::Dblp::Fields::BOOKTITLE)
		mBooktitle = mLastText;
	else if (localName == Const::Dblp::Fields::CROSSREF)
		mCrossref = mLastText;

	else if (localName == Const::Dblp::Fields::JOURNAL) {
		// This is a special case since a journal entity doesn't exists
		// in dblm.xml; but we notify a journal only for its name on an article
		// element
		DblpJournal journal;
		journal.name = mLastText;
		mHandler->onJournal(journal, mInputFile.pos());
	}

	return true;
}

bool DblpXmlParser::characters(const QString &chars)
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

bool DblpXmlParser::ignorableWhitespace(const QString &ch)
{
//	vv("ignorableWhitespace: " << ch);

	return true;
}

bool DblpXmlParser::processingInstruction(const QString &target, const QString &data)
{
//	vv("processingInstruction: " << target << " | " << data);

	return true;
}

bool DblpXmlParser::skippedEntity(const QString &name)
{
	QString entityString = Util::Html::entityNameToString(name);
	vv("skippedEntity: " << name);
	vv("skippedEntity to qstring: " << entityString);

	mLastText += entityString;

	return true;
}

bool DblpXmlParser::error(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

bool DblpXmlParser::fatalError(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

bool DblpXmlParser::warning(const QXmlParseException &exception)
{
	ee("Error occurred while parsing document: " << exception.message());

	return true;
}

QString DblpXmlParser::errorString() const
{
	return "PARSE ERROR";
}
