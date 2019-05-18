#include "gui_dblp_xml_lines.h"

QHash<int, QByteArray> GuiDblpXmlLines::GUI_DBLP_XML_LINE_ROLES = {
	{GuiDblpXmlLineRoleType, "type"},
	{GuiDblpXmlLineRoleTag, "tag"},
	{GuiDblpXmlLineRoleContent, "content"},
	{GuiDblpXmlLineRoleTagAttributes, "attributes"},
	{GuiDblpXmlLineRoleTagIndent, "indent"},
	{GuiDblpXmlLineRoleTagCrossref, "crossref"},
};

GuiDblpXmlLines::GuiDblpXmlLines(QObject *parent) : QAbstractListModel (parent)
{
}

void GuiDblpXmlLines::addLine(const GuiDblpXmlLine &line)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	mLines.append(line);
	endInsertRows();
}

void GuiDblpXmlLines::addLines(const QVector<GuiDblpXmlLine> &lines)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount() + lines.size() - 1);
	for (const GuiDblpXmlLine& line : lines)
		mLines.append(line);
	endInsertRows();
}

void GuiDblpXmlLines::clearLines()
{
	beginRemoveRows(QModelIndex(), 0, rowCount());
	mLines.clear();
	endRemoveRows();
}

int GuiDblpXmlLines::size()
{
	return mLines.size();
}

int GuiDblpXmlLines::rowCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return mLines.size();
}

QVariant GuiDblpXmlLines::data(const QModelIndex &index, int role) const
{
	if (index.row() < 0 || index.row() >= mLines.size())
		return QVariant();

	const GuiDblpXmlLine &line = mLines.at(index.row());

	switch (role) {
	case GuiDblpXmlLineRoleType:
		return line.type();
	case GuiDblpXmlLineRoleTag:
		return line.tag();
	case GuiDblpXmlLineRoleContent:
		return line.content();
	case GuiDblpXmlLineRoleTagAttributes:
		return line.attributes();
	case GuiDblpXmlLineRoleTagIndent:
		return line.indent();
	case GuiDblpXmlLineRoleTagCrossref:
		return line.crossref();
	}

	return QVariant();
}

QHash<int, QByteArray> GuiDblpXmlLines::roleNames() const
{
	return GUI_DBLP_XML_LINE_ROLES;
}
