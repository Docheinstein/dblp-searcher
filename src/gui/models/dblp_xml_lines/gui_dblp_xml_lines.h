#ifndef GUI_DBLP_XML_LINES_H
#define GUI_DBLP_XML_LINES_H

#include <QAbstractListModel>

#include <gui/models/dblp_xml_line/gui_dblp_xml_line.h>

class GuiDblpXmlLines : public QAbstractListModel {

	Q_OBJECT

public:
	GuiDblpXmlLines(QObject *parent = nullptr);

	void addLine(const GuiDblpXmlLine &line);
	void addLines(const QVector<GuiDblpXmlLine> &line);
	void clearLines();

	void notifyDataChanged();

	int size();

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
	};

	static QHash<int, QByteArray> GUI_DBLP_XML_LINE_ROLES;

	QVector<GuiDblpXmlLine> mLines;
};
#endif // GUI_DBLP_XML_LINES_H
