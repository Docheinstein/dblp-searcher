#ifndef GUI_DBLP_XML_LINES_H
#define GUI_DBLP_XML_LINES_H

#include <QAbstractListModel>

#include "gui/components/dblp_xml_line/gui_dblp_xml_line.h"

class GuiDblpXmlLines : public QAbstractListModel {

	Q_OBJECT

public:
	GuiDblpXmlLines(QObject *parent = nullptr);

	void addLine(const GuiDblpXmlLine &line);
	void addLines(const QVector<GuiDblpXmlLine> &lines);
	void clearLines();

	int size() const;

	// QAbstractItemModel interface
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QHash<int, QByteArray> roleNames() const;

private:
	enum GuiDblpXmlLineRole {
		GuiDblpXmlLineRoleType = Qt::UserRole + 1,
		GuiDblpXmlLineRoleTag,
		GuiDblpXmlLineRoleContent,
		GuiDblpXmlLineRoleTagAttributes,
		GuiDblpXmlLineRoleTagIndent,
		GuiDblpXmlLineRoleTagCrossref,
	};

	static QHash<int, QByteArray> GUI_DBLP_XML_LINE_ROLES;

	QVector<GuiDblpXmlLine> mLines;
};

#endif // GUI_DBLP_XML_LINES_H
