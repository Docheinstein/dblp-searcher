#include "gui_element_details.h"
#include "commons/globals/globals.h"
#include <QString>
#include <QTextDocument>
#include <QtConcurrent>
#include "main/args.h"
#include "dblp/xml/parser/dblp_xml_parser.h"
#include "commons/const/const.h"
#include "gui/main/gui_main_window.h"
#include "dblp/query/resolver/query_resolver.h"

#define HIGHLIGHT_COLOR "#e24834"
#define HIGHLIGHTED(term) "<b style='color: " HIGHLIGHT_COLOR "'>" + term + "</b>"

LOGGING(GuiElementDetails, true)

DblpElementXmlRetrieverHandler::~DblpElementXmlRetrieverHandler() {}
GuiElementDetails::~GuiElementDetails() {}

GuiElementDetails::GuiElementDetails()
{
	QObject::connect(this,
					 SIGNAL(elementRetrieved()),
					 this,
					 SLOT(xmlLoadingFinished()));
}

int GuiElementDetails::serial()
{
	return INT(mSerial);
}

void GuiElementDetails::setSerial(int serial)
{
	bool toInit = static_cast<elem_serial>(serial) != mSerial;

	mSerial = static_cast<elem_serial>(serial);
	emit serialChanged();

	if (toInit) {
		init();
	} else {
		dd("Skipping creation of element details for serial: " << serial);
	}
}

//int GuiElementDetails::crossrefSerial()
//{
//	return INT(mCrossrefSerial);
//}

QString GuiElementDetails::identifier()
{
	return mIdentifier;
}

QObject * GuiElementDetails::xmlLines()
{
	return &mLines;
}

QObject *GuiElementDetails::publications()
{
	return &mPublications;
}

int GuiElementDetails::publicationsCount()
{
	return mPublications.size();
}

bool GuiElementDetails::hasXml()
{
	return mHasXml;
}

bool GuiElementDetails::hasPublications()
{
	return mHasPublications;
}

void GuiElementDetails::onElementRetrieved(const DblpXmlElement &elem)
{
	vv1("Retrieved XML element from original dblp file: " << endl << elem);

	// Retrieve the matches (for highlight, later)
	QVector<IndexMatch> indexMatches =
			GuiMainWindow::instance().elementMatches(mSerial);

	auto highlightedFieldValue = [&indexMatches, this]
			(const QString &fieldName, const QString &fieldValue,
			int fieldNum) -> QString /* eventually highlighted fieldValue */ {

		_dd("Checking highlighting for field: " << fieldName
			 << "[" << fieldNum << "]: " << fieldValue);

		// Highlight the query matches, we just have a vector, so scan all the
		// vector and eventually highlight the matches
		QStringList fieldTerms = fieldValue
				.split(" ", QString::SplitBehavior::SkipEmptyParts);

		QStringList highlightedFieldValues;

		auto fieldHasIndexMatches =
				[&fieldName](const IndexMatch &indexMatch) {
				ElementFieldTypes fieldTypeFlag = indexMatch.fieldType;

				return (fieldName == Const::Dblp::Xml::Fields::AUTHOR &&
						(fieldTypeFlag & ElementFieldType::Author) == fieldTypeFlag) ||
						(fieldName == Const::Dblp::Xml::Fields::YEAR &&
						(fieldTypeFlag & ElementFieldType::Year) == fieldTypeFlag) ||
						(fieldName == Const::Dblp::Xml::Fields::TITLE &&
						(fieldTypeFlag & ElementFieldType::Title) == fieldTypeFlag) ||
	//					(fieldName == Const::Dblp::Xml::Fields::BOOKTITLE &&
	//					fieldTypeFlag.testFlag(ElementFieldType::Booktitle) ||
						(fieldName == Const::Dblp::Xml::Fields::PUBLISHER &&
						(fieldTypeFlag & ElementFieldType::Publisher) == fieldTypeFlag) ||
						(fieldName == Const::Dblp::Xml::Fields::JOURNAL &&
						(fieldTypeFlag & ElementFieldType::Journal) == fieldTypeFlag)

						;
		};

		for (int termPos = 0; termPos < fieldTerms.size();) {
			_dd1("Scanning field term: " << fieldTerms.at(termPos));

			bool highlighted = false;

			for (const IndexMatch & indexMatch : indexMatches) {
				_dd3("Match term");
				_dd3("- term pos: " << indexMatch.matchPosition);
				_dd3("- field num: " << indexMatch.fieldNumber);
				_dd3("- field type: " << elementFieldTypeString(indexMatch.fieldType));

				if (/* mSerial == indexMatch.elementSerial && */
					termPos == indexMatch.matchPosition &&
					fieldNum == indexMatch.fieldNumber &&
					fieldHasIndexMatches(indexMatch)) {
					// This field is the one referred by the IndexMatch, highlight
					// the words in the positions of the matches
					// Since the matches could be phrasal, highlight
					// a count of terms equals to the matches tokens length
					int termPosBegin = termPos;

					_dd4("Highlighting streak of terms from: " <<
						 fieldTerms.at(termPos) << " w/ length: " <<
						 indexMatch.matchedTokens.size());

					while (	termPos - termPosBegin < indexMatch.matchedTokens.size() &&
							termPos < fieldTerms.size()) // theorically not needed...
					{
						_dd5("Highlighting term: " << fieldTerms.at(termPos));
						highlightedFieldValues.append(HIGHLIGHTED(fieldTerms.at(termPos)));
						++termPos;
					};

					highlighted = true;
					break;
				}
			}

			if (!highlighted) {
				_dd3("Non highlighting term: " << fieldTerms.at(termPos));
				highlightedFieldValues.append(fieldTerms.at(termPos));
				++termPos;
			}
		}

		return highlightedFieldValues.join(" ");
	};

	// Push the element as XML lines

	// Compute attributes
	QStringList attrs;
	for (const QXmlStreamAttribute & attr : elem.attributes) {
		attrs.append(attr.name() + "=\"" + attr.value() + "\"");
	}

	// Element open
	mLinesRaw.append(
		GuiDblpXmlLine::Builder()
				.tag(elem.name)
				.indent(false)
				.type(GuiDblpXmlLineType::Open)
				.attributes(attrs.join(" "))
				.build()
	);

	// Fields
	for (auto it = elem.fields.cbegin(); it != elem.fields.cend(); it++) {
		const QString fieldName = it.key();
		const QVector<QString> fieldValues = it.value();

		int fieldNum = 0;

		for (const QString & fieldValue : fieldValues) {
			GuiDblpXmlLine::Builder lineBuilder;
			lineBuilder
				.tag(fieldName)
				.indent(true)
				.type(GuiDblpXmlLineType::Inline)
				.content(highlightedFieldValue(fieldName, fieldValue, fieldNum))
				.crossref(-1);


			// Mark the crossrefs rows

			if (mHasCrossref &&
				(fieldName == Const::Dblp::Xml::Fields::CROSSREF
				// || fieldName == Const::Dblp::Xml::Fields::JOURNAL)
				))
				lineBuilder.crossref(INT(mCrossrefSerial));

			mLinesRaw.append(lineBuilder.build());

			++fieldNum;
		}
	}

	// Element close
	mLinesRaw.append(
		GuiDblpXmlLine::Builder()
				.tag(elem.name)
				.indent(false)
				.type(GuiDblpXmlLineType::Close)
				.build()
	);


	vv("XML loading finished; # lines " << mLinesRaw.size());
	// The actually push to mLines is done by the gui thread
	emit elementRetrieved();
}

void GuiElementDetails::xmlLoadingFinished()
{
	// Push the raw lines to the real gui lines
	mLines.addLines(mLinesRaw);
	emit xmlLinesChanged();

	// Already fired
	// emit hasXmlChanged();
}

void GuiElementDetails::init()
{
	ASSERT(mSerial >= 0, "gui",
		   "Cannot instantiate element details for serial < 0");

	dd("Creating element details component for serial: " << mSerial);

	// Load identifier
	GuiMainWindow::instance().resolver()->irModel()->index()
			.identifier(mSerial, mIdentifier);

	emit identifierChanged();

	// Load crossref

	mHasCrossref = GuiMainWindow::instance().resolver()->irModel()->index()
			.crossref(mSerial, mCrossrefSerial);

//	emit crossrefSerialChanged();

	// Load XML
	if (!arguments.dblpXmlFilePath.isEmpty()) {
		QFuture<void> xmlLoadingFuture =
				QtConcurrent::run(this, &GuiElementDetails::loadXml);
		mHasXml = true;
		emit hasXmlChanged();
		// GUI hack: notify now so that the XML tab will have position 0
	}

	//  Load publications, if those are available for the element (only for venues)
	QVector<elem_serial> publications;
	if (GuiMainWindow::instance().resolver()->irModel()->index()
			.inverseCrossref(mSerial, publications)) {

		QVector<GuiElementPublication> guiPublications;

		for (elem_serial &pubSerial : publications) {
			guiPublications.append(
						GuiElementPublication::Builder()
						.serial(pubSerial)
						.build()
			);
		}

		mPublications.addPublications(guiPublications);

		mHasPublications = true;

		emit publicationsCountChanged();
		emit hasPublicationsChanged();

		emit publicationsChanged();
	}
}

void GuiElementDetails::loadXml()
{
	dd("Retrieving XML from: " << arguments.dblpXmlFilePath);

	// Create a new retriever for this element that starts at the position
	// told by the index handler (start from the lower bound)
	QPair<elem_pos, elem_pos> elementPosition;
	if (!GuiMainWindow::instance().resolver()->irModel()->index()
			.positionRange(mSerial, elementPosition)) {
		ww("Cannot retrieve original position for element: [" << mSerial << "]");
		return;
	}

	dd("Element position range (LB: " << elementPosition.first << " | " <<
							" UB: " << elementPosition.second << ")");

	DblpElementXmlRetriever retriever(mSerial, *this);
	DblpXmlParser parser(arguments.dblpXmlFilePath, retriever,
						 elementPosition.first, elementPosition.second);
	parser.parse();
}

DblpElementXmlRetriever::DblpElementXmlRetriever(elem_serial serial,
												 DblpElementXmlRetrieverHandler &handler)
	: mSerial(serial), mHandler(handler)
{
	// Compute the identifier too
	GuiMainWindow::instance().resolver()->irModel()->
			index().identifier(mSerial, mIdentifier);
}

void DblpElementXmlRetriever::onParseStart()
{
	_dd("Started XML parsing for retrieve original XML of element [" << mSerial << "]");
}

void DblpElementXmlRetriever::onParseEnd()
{

}

bool DblpElementXmlRetriever::onElement(
		const DblpXmlElement &element, qint64 pos)
{
	Q_UNUSED(pos)

	const QStringRef &elementIdentifier =
			element.attributes.value(Const::Dblp::Xml::Attributes::KEY);

	if (elementIdentifier != mIdentifier) {
		_dd1("Skipping element " << elementIdentifier);
		return true;
	}

	// Read the right element

	mHandler.onElementRetrieved(element);

	return false;
}
