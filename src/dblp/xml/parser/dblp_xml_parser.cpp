#include "dblp_xml_parser.h"
#include "dblp/xml/models/types/dblp_xml_types.h"
#include "commons/const/const.h"
#include "commons/util/util.h"
#include "commons/profiler/profiler.h"
#include <QThread>

LOGGING(DblpXmlParser, true)

#define DUMP_XML 1

static const QString DBLP_XML_START_OF_DOCUMENT =
R"#(<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE dblp SYSTEM "dblp.dtd">
<dblp>)#";

namespace DblpXmlElements = Const::Dblp::Xml::Elements;
namespace DblpXmlFields = Const::Dblp::Xml::Fields;
namespace DblpXmlAttributes = Const::Dblp::Xml::Attributes;

DblpXmlParser::DblpXmlParser(const QString &inputFilePath,
							 DblpXmlParseHandler &parseHandler,
							 qint64 startingPosition, qint64 endingPosition)
	: mHandler(parseHandler)
{
	ASSERT(endingPosition >= startingPosition, "parsing",
		   "Cannot parse a document with ending position "
		   "not greater than starting position");

	mInput.setFileName(inputFilePath);
	mStartingPosition = startingPosition;
	mEndingPosition = endingPosition;
}

bool DblpXmlParser::parse()
{
	PROF_FUNC_BEGIN

	bool continueParsing = true;

	if (!setParserDevice()) {
		ee("Error occurred while opening XML file");
		return false;
	}

	ii("Starting XML file parsing of: " << mInput.fileName());

	mHandler.onParseStart();

	while (!mReader.atEnd() &&
		   !mReader.hasError() &&
		   continueParsing /* stopped by the handler? */) {

		mReader.readNext();

#if DUMP_XML
		dd(mReader.tokenString());
#endif

		switch (mReader.tokenType()) {
		case QXmlStreamReader::StartElement:
			continueParsing &= handleStartElement();
			break;
		case QXmlStreamReader::EndElement:
			continueParsing &= handleEndElement();
			break;
		case QXmlStreamReader::Characters:
#if DUMP_XML
			dd1(mReader.text());
#endif
			break;
		default:
			break;
		}

		// Notify progress
		notifyProgress();
	}

	if (mReader.hasError()) {
		ee("Error occurred while parsing XML file: " << mReader.errorString());
	}

	if (!continueParsing) {
		dd("Parser has been stopped by the handler");
	}

	mHandler.onParseEnd();

	PROF_FUNC_END

	return !mReader.hasError();
}

bool DblpXmlParser::handleStartElement()
{
	bool continueParsing = true;
	const QString &token = mReader.name().toString();

#if DUMP_XML
	dd1(token);
#endif

	if (token == DblpXmlElements::ARTICLE ||
		token == DblpXmlElements::INCOLLECTION ||
		token == DblpXmlElements::BOOK ||
		token == DblpXmlElements::INPROCEEDINGS ||
		token == DblpXmlElements::PROCEEDINGS ||
		token == DblpXmlElements::PHDTHESIS ||
		token == DblpXmlElements::MASTERTHESIS) {

		// Save the element name and the attributes
		mElementName = token;
		mElementAttributes = mReader.attributes();

	} else if (token != Const::Dblp::Xml::DBLP){
		// Read every possible field, so that even for purpose different from
		// indexing the entire XML element could be figured out again

//		if (
//		token == DblpXmlFields::AUTHOR ||
//		token == DblpXmlFields::TITLE ||
//		token == DblpXmlFields::YEAR ||
//		token == DblpXmlFields::PUBLISHER ||
//		token == DblpXmlFields::CROSSREF ||
//		token == DblpXmlFields::BOOKTITLE ||
//		token == DblpXmlFields::JOURNAL) {

		continueParsing &= handleStartField(token);
	}

	return continueParsing;
}

bool DblpXmlParser::handleStartField(const QString &field)
{
	QString innerToken;
	QString fieldContent;

	bool fieldContentRead = false;

	do {
		mReader.readNext();

		#if DUMP_XML
			dd3(mReader.tokenString());
		#endif

		switch (mReader.tokenType()) {
		case QXmlStreamReader::Characters:
			#if DUMP_XML
				dd4(mReader.text());
			#endif
			fieldContent += mReader.text();
			break;
		case QXmlStreamReader::EntityReference:
			#if DUMP_XML
				dd4(mReader.name());
				dd5(Util::Html::entityNameToString(mReader.name().toString()););
			#endif
			fieldContent += Util::Html::entityNameToString(mReader.name().toString());
			break;
		case QXmlStreamReader::EndElement:
			if (mReader.name() == field) {
				fieldContentRead = true; // ok, found expected end tag
			}
			else {
//				#if DUMP_XML
//					ww("Unexpected tag found for field '" + field + "'");
//				#endif
				// Go on and hope...
			}
			break;
		case QXmlStreamReader::Invalid:
			#if DUMP_XML
				ww("Invalid; error: " << mReader.errorString());
			#endif
			break;
		default:
			break;
		}
	} while(!mReader.atEnd() &&
			!mReader.hasError() &&
			!fieldContentRead);

	// Save the token
#if DUMP_XML
	dd2(fieldContent);
#endif

	auto fieldIt = mElementFields.find(field);

	if (fieldIt == mElementFields.end()) {
		// Insert for the first time
		mElementFields.insert(field, {fieldContent});
	} else {
		// Retrieve the string list and add it
		fieldIt.value().append(fieldContent);
	}

	return !mReader.hasError();
}

bool DblpXmlParser::handleEndElement(bool resetState)
{
	bool continueParsing = true;

	const QString &token = mReader.name().toString();

#if DUMP_XML
	dd1(token);
#endif

	if (token == DblpXmlElements::ARTICLE ||
		token == DblpXmlElements::INCOLLECTION ||
		token == DblpXmlElements::BOOK ||
		token == DblpXmlElements::INPROCEEDINGS ||
		token == DblpXmlElements::PROCEEDINGS ||
		token == DblpXmlElements::PHDTHESIS ||
		token == DblpXmlElements::MASTERTHESIS) {

		// Save the element name and the attributes
		DblpXmlElement elem;
		elem.name = mElementName;
		elem.attributes = mElementAttributes;
		elem.fields = mElementFields;

		continueParsing = mHandler.onElement(elem, mInput.pos()
								/* Upper bound, who knows the exact pos...*/);
		if (resetState)
			resetParserState();
	}

	return continueParsing;
}

void DblpXmlParser::notifyProgress()
{
	if (mStartingPosition != 0 || mEndingPosition != 0)
		return; // Nothing to notify for buffer parsing

	const qint64 INPUT_FILE_SIZE = mInput.size();

	int perc = INT((DOUBLE(mInput.pos()) / INPUT_FILE_SIZE) * 100);

	if (perc > mLastNotifiedPercentage) {
		mLastNotifiedPercentage = perc;
		ii("Parsing progress: " << mLastNotifiedPercentage << "%");
	}
}

bool DblpXmlParser::setParserDevice()
{
	if (mStartingPosition > 0)
		return setParserDeviceBuffer();
	else
		return setParserDeviceFile();
}

bool DblpXmlParser::setParserDeviceFile()
{
	dd("Setting parser device: file");

	if (!mInput.open(QIODevice::ReadOnly))
		return false;

	mReader.setDevice(&mInput);

	return true;
}

bool DblpXmlParser::setParserDeviceBuffer()
{
	dd("Setting parser device: buffer");

	if (!mInput.open(QIODevice::ReadOnly))
		return false;

	// Read the file until a token that makes sense is read, then start the parse
	// (can be useful for start in the middle of an XML without known the
	// exact position of an element)

	// Go the the specified position, if required
	if (mStartingPosition > 0) {
		dd("Seeking on input file to position: " << mStartingPosition);
		mInput.seek(mStartingPosition);
	}

	const qint64 BUF_SIZE = mEndingPosition - mStartingPosition;
	char * data = new char[BUF_SIZE];
	mInput.read(data, BUF_SIZE);

	QString qdata = QString::fromUtf8(data);

	dd("Read raw data from file: " << qdata);

	// Found the first token that delimits a start of tag for a known element

	// Where are them?
	int debrisMinSize = INT_MAX;
	int articleOffset = qdata.indexOf("<" + DblpXmlElements::ARTICLE);
	int incollectionOffset = qdata.indexOf("<" + DblpXmlElements::INCOLLECTION);
	int bookOffset = qdata.indexOf("<" + DblpXmlElements::BOOK);
	int inprocOffset = qdata.indexOf("<" + DblpXmlElements::INPROCEEDINGS);
	int procOffset = qdata.indexOf("<" + DblpXmlElements::PROCEEDINGS);
	int phdOffset = qdata.indexOf("<" + DblpXmlElements::PHDTHESIS);
	int masterOffset = qdata.indexOf("<" + DblpXmlElements::MASTERTHESIS);


	if (articleOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, articleOffset);
	if (incollectionOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, incollectionOffset);
	if (bookOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, bookOffset);
	if (inprocOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, inprocOffset);
	if (procOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, procOffset);
	if (phdOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, phdOffset);
	if (masterOffset >= 0)
		debrisMinSize = MIN(debrisMinSize, masterOffset);

	if (debrisMinSize == INT_MAX) {
		ww("Cannot find valid element's open tag within " << BUF_SIZE << " bytes ");
		return false;
	}

	dd("Skipping debris bytes (chars): " << debrisMinSize);

	// Ok, now we have to read starting from the right position and we have
	// to prepend a valid start of document to the read string

	QString validXml = DBLP_XML_START_OF_DOCUMENT + qdata.mid(debrisMinSize);

	mReader.addData(validXml);

	delete[] data;

	return true;
}

void DblpXmlParser::resetParserState()
{
	mElementAttributes.clear();
	mElementFields.clear();
}
