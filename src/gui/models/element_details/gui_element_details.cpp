#include "gui_element_details.h"
#include "commons/globals/globals.h"
#include <QString>
#include <QTextDocument>
#include <QtConcurrent>

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

GuiElementDetails::GuiElementDetails()
{
	QObject::connect(&mXmlLoadingWatcher,
					 SIGNAL(finished()),
					 this,
					 SLOT(xmlLoadingFinished()));
}

int GuiElementDetails::serial()
{
	return INT(mSerial);
}

void GuiElementDetails::setSerial(int serial)
{
	dd("Called GuiElementDetails.setElementSerial() for serial: " << serial);
	mSerial = static_cast<elem_serial>(serial);
	emit serialChanged();

	QFuture<void> xmlLoadingFuture = QtConcurrent::run(this, &GuiElementDetails::loadXml);
	mXmlLoadingWatcher.setFuture(xmlLoadingFuture);
}

QObject * GuiElementDetails::xmlLines()
{
	return &mLines;
}

void GuiElementDetails::xmlLoadingFinished()
{
	ii("XML loading finished = " << mLinesRaw.size());
	mLines.addLines(mLinesRaw);
	emit xmlLinesChanged();
}

void GuiElementDetails::loadXml()
{
	dd("Retrieving XML...");
//	QThread::sleep(2);
	mLinesRaw.append(
				GuiDblpXmlLine::Builder()
				.indent(false)
				.tag("article")
				.type(GuiDblpXmlLineType::Type::Open)
				.attributes("key=\"journals/it/hackaton-sols\" mdate=\"2019-05-27\"")
				.content("")
				.build()
	);
	mLinesRaw.append(
				GuiDblpXmlLine::Builder()
				.indent(true)
				.tag("author")
				.type(GuiDblpXmlLineType::Type::Inline)
				.content("Enrico <b>Sedoni</b> information")
				.build()
	);
	mLinesRaw.append(
				GuiDblpXmlLine::Builder()
				.indent(false)
				.tag("article")
				.type(GuiDblpXmlLineType::Type::Close)
				.build()
	);
}
