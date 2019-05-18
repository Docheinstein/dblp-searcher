#include "gui_dblp_xml_line.h"

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::indent(bool indent)
{
	mIndent = indent;
	return *this;
}

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::type(GuiDblpXmlLineType::Type type)
{
	mType = type;
	return *this;
}

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::tag(QString tag)
{
	mTag = tag;
	return *this;
}

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::attributes(QString attrs)
{
	mAttributes = attrs;
	return *this;
}

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::content(QString content)
{
	mContent = content;
	return *this;
}

GuiDblpXmlLine::Builder &GuiDblpXmlLine::Builder::crossref(int crossref)
{
	mCrossref = crossref;
	return *this;
}

GuiDblpXmlLine GuiDblpXmlLine::Builder::build()
{
	GuiDblpXmlLine line;
	line.mIndent = mIndent;
	line.mType = mType;
	line.mTag = mTag;
	line.mAttributes = mAttributes;
	line.mContent = mContent;
	line.mCrossref = mCrossref;
	return line;
}

bool GuiDblpXmlLine::indent() const
{
	return mIndent;
}

GuiDblpXmlLineType::Type GuiDblpXmlLine::type() const
{
	return mType;
}

QString GuiDblpXmlLine::tag() const
{
	return mTag;
}

QString GuiDblpXmlLine::attributes() const
{
	return mAttributes;
}

QString GuiDblpXmlLine::content() const
{
	return mContent;
}

int GuiDblpXmlLine::crossref() const
{
	return mCrossref;
}
