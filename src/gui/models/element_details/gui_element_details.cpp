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

const char * const XML_EXAMPLE = R"#(
<article key="journals/it/hackaton-sols" mdate="2019-05-27"\>$$
^^<author>Amarjon Singh</author>$$
^^<author>Lorenzo Miniero</author>$$
^^<title>Hackaton solutions for information retrieval</title>$$
^^<year>2019</year>$$
^^<journal>Computer Science (IT)</journal>$$
</article>
)#";

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
	ASSERT(serial >= 0, "gui",
		   "Cannot instantiate element details for serial < 0");

	dd("Creating element details component for serial: " << serial);

	mSerial = static_cast<elem_serial>(serial);

	emit serialChanged();

	// Load identifier
	GuiMainWindow::instance().resolver()->irModel()->index()
			.identifier(mSerial, mIdentifier);

	emit identifierChanged();

	// Load XML
	if (!arguments.dblpXmlFilePath.isEmpty()) {
		QFuture<void> xmlLoadingFuture =
				QtConcurrent::run(this, &GuiElementDetails::loadXml);
	}

	//  Load publications, if those are available for the element (only for venues)
	QVector<elem_serial> publications;
	if (GuiMainWindow::instance().resolver()->irModel()->index()
			.inverseCrossref(static_cast<elem_serial>(serial), publications)) {

		QVector<GuiElementPublication> guiPublications;

		for (elem_serial &pubSerial : publications) {
			guiPublications.append(
						GuiElementPublication::Builder()
						.serial(pubSerial)
						.build()
			);
		}

		mPublications.addPublications(guiPublications);

		emit publicationsChanged();
	}

	if (mPublications.size() > 0) {
		emit hasPublicationsChanged();
	}
}

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

bool GuiElementDetails::hasPublications()
{
	return mPublications.size() > 0;
}

void GuiElementDetails::onElementRetrieved(const DblpXmlElement &elem)
{
	vv1("Retrieved XML element from original dblp file: " << endl << elem);

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
		const QVector<QString> fieldValues = it.value();
		for (const QString & fieldValue : fieldValues) {
			mLinesRaw.append(
				GuiDblpXmlLine::Builder()
						.tag(it.key())
						.indent(true)
						.type(GuiDblpXmlLineType::Inline)
						.content(fieldValue)
						.build()
			);
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
						 elementPosition.first);
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
