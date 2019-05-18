#include "dblp_xml_parser.h"
#include "dblp/xml/models/types/dblp_xml_types.h"
#include "commons/const/const.h"
#include "commons/util/util.h"
#include "commons/profiler/profiler.h"

LOGGING(DblpXmlParser, true)

#define DUMP_XML 0

namespace DblpXmlElements = Const::Dblp::Xml::Elements;
namespace DblpXmlFields = Const::Dblp::Xml::Fields;
namespace DblpXmlAttributes = Const::Dblp::Xml::Attributes;

DblpXmlParser::DblpXmlParser(const QString &inputFilePath, DblpXmlParseHandler &parseHandler,
							 qint64 startingPosition)
	: mHandler(parseHandler)
{
	mInput.setFileName(inputFilePath);
	mStartingPosition = startingPosition;
}

bool DblpXmlParser::parse()
{
	PROF_FUNC_BEGIN

	const qint64 INPUT_FILE_SIZE = mInput.size();
	bool continueParsing = true;

	ii("Starting XML file parsing of: " << mInput.fileName());

	if (!mInput.open(QIODevice::ReadOnly)) {
		ee("Error occurred while opening XML file");
		return false;
	}

	// Go the the specified position, if required
	if (mStartingPosition > 0) {
		dd("Seeking on input file to position: " << mStartingPosition);
		mInput.seek(mStartingPosition);
	}

	mReader.setDevice(&mInput);

	mHandler.onParseStart();

	while (!mReader.atEnd() && !mReader.hasError() &&
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
		return false;
	}

	if (!continueParsing) {
		dd("Parser has been stopped by the handler");
	}

	mHandler.onParseEnd();

	PROF_FUNC_END

	return true;
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

void DblpXmlParser::resetParserState()
{
	mElementAttributes.clear();
	mElementFields.clear();
}
