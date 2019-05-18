#include "dblp_xml_parser.h"
#include "dblp/xml/models/types/dblp_xml_types.h"
#include "commons/const/const.h"
#include "commons/util/util.h"
#include "commons/profiler/profiler.h"
#include <QThread>

LOGGING(DblpXmlParser, true)

#define DUMP_XML 1

static const char * const DBLP_XML_START_OF_DOCUMENT =
R"#(<?xml version="1.0" encoding="ISO-8859-1"?>
<!DOCTYPE dblp SYSTEM "dblp.dtd">
<dblp>
)#";

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

	const qint64 INPUT_FILE_SIZE = mInput.size();
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
//		dd("bytesAvailable: " << mInput.bytesAvailable());
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
		default:
//			QThread::sleep(1);
			break;
		}

		// Notify progress

		int perc = INT((DOUBLE(mInput.pos()) / INPUT_FILE_SIZE) * 100);

		if (perc > mLastNotifiedPercentage) {
			mLastNotifiedPercentage = perc;
			ii("Parsing progress: " << mLastNotifiedPercentage << "%");
		}
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

	} else if (
		token == DblpXmlFields::AUTHOR ||
		token == DblpXmlFields::TITLE ||
		token == DblpXmlFields::YEAR ||
		token == DblpXmlFields::PUBLISHER ||
		token == DblpXmlFields::CROSSREF ||
		token == DblpXmlFields::JOURNAL) {

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
			if (mReader.name() == field)
				fieldContentRead = true; // ok, found expected end tag
			else {
				#if DUMP_XML
					ww("Unexpected tag found for field '" + field + "'");
				#endif
				// Go on and hope...
			}
			break;
		case QXmlStreamReader::Invalid:
			#if DUMP_XML
				dd4(mReader.text());
			#endif
//				QThread::sleep(1);
			break;
		default:
			break;
		}
	} while(!fieldContentRead);

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

	return true;
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

		continueParsing = mHandler.onElement(elem, mInput.pos());
		if (resetState)
			resetParserState();
	}

	return continueParsing;
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

	dd("Read raw data from file: " << data);

	// Found the first token that delimits a start of tag for a known element

	// Where are them?
	char * articlePos = strstr(data,
			QString("<" + DblpXmlElements::ARTICLE).toStdString().c_str());
	char * incollectionPos = strstr(data,
			QString("<" + DblpXmlElements::INCOLLECTION).toStdString().c_str());
	char * bookPos = strstr(data,
			QString("<" + DblpXmlElements::BOOK).toStdString().c_str());
	char * inprocPos = strstr(data,
			QString("<" + DblpXmlElements::INPROCEEDINGS).toStdString().c_str());
	char * procPos = strstr(data,
			QString("<" + DblpXmlElements::PROCEEDINGS).toStdString().c_str());
	char * phdPos = strstr(data,
			QString("<" + DblpXmlElements::PHDTHESIS).toStdString().c_str());
	char * masterPos = strstr(data,
			QString("<" + DblpXmlElements::MASTERTHESIS).toStdString().c_str());

	// What's the offset with the beginning of the chunk?
	long articleOffset = articlePos ? (articlePos - data) : LONG_MAX;
	long incollectionOffset = incollectionPos ? (incollectionPos - data) : LONG_MAX;
	long bookOffset = bookPos ? (bookPos - data) : LONG_MAX;
	long inprocOffset = inprocPos ? (inprocPos - data) : LONG_MAX;
	long procOffset = procPos ? (procPos - data) : LONG_MAX;
	long phdOffset = phdPos ? (phdPos - data) : LONG_MAX;
	long masterOffset = masterPos ? (masterPos - data) : LONG_MAX;

	// Which one comes first?
	long debrisMinSize = articleOffset;
	debrisMinSize = debrisMinSize < incollectionOffset ? debrisMinSize : incollectionOffset;
	debrisMinSize = debrisMinSize < bookOffset ? debrisMinSize : bookOffset;
	debrisMinSize = debrisMinSize < inprocOffset ? debrisMinSize : inprocOffset;
	debrisMinSize = debrisMinSize < procOffset ? debrisMinSize : procOffset;
	debrisMinSize = debrisMinSize < phdOffset ? debrisMinSize : phdOffset;
	debrisMinSize = debrisMinSize < masterOffset ? debrisMinSize : masterOffset;

	if (debrisMinSize == LONG_MAX) {
		ww("Cannot find valid element's open tag within " << BUF_SIZE << " bytes ");
		return false;
	}

	dd("Skipping debris bytes: " << debrisMinSize);

	// Ok, now we have to read starting from the right position and we have
	// to prepend a valid start of document to the read string

	mInput.seek(mStartingPosition + debrisMinSize);

	char * exactData = new char[BUF_SIZE];
	mInput.read(exactData, BUF_SIZE);

	QString dummyXml = DBLP_XML_START_OF_DOCUMENT + QString(exactData);

	mReader.addData(dummyXml);

	return true;
}

//void DblpXmlParser::skipStartingDebris()
//{
//	static const int ALLOWED_DEBRIS_MAX_SIZE = 4096;

//	// Start from the starting position
//	if (mStartingPosition > 0) {
//		mInput.seek(mStartingPosition);
//	}

	// Read a chunk from the file

//}

void DblpXmlParser::resetParserState()
{
	mElementAttributes.clear();
	mElementFields.clear();
}
