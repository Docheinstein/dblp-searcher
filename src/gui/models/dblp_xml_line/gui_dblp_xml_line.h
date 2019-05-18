#ifndef GUI_DBLP_XML_LINE_H
#define GUI_DBLP_XML_LINE_H

#include <QAbstractListModel>

#define GUI_DBLP_XML_LINE_QML_NAME "DblpXmlLine"

class GuiDblpXmlLineType : public QObject {
	Q_OBJECT

public:
	enum Type {
		Open,
		Close,
		Inline
	};
	Q_ENUM(Type)
};

class GuiDblpXmlLine {

public:
	class Builder {
	public:
		Builder & indent(bool indent);
		Builder & type(GuiDblpXmlLineType::Type type);
		Builder & tag(QString tag);
		Builder & attributes(QString attrs);
		Builder & content(QString content);
		Builder & crossref(int crossref);

		GuiDblpXmlLine build();

	private:
		bool mIndent;
		GuiDblpXmlLineType::Type mType;
		QString mTag;
		QString mAttributes;
		QString mContent;
		int mCrossref;
	};

	bool indent() const;
	GuiDblpXmlLineType::Type type() const;
	QString tag() const;
	QString attributes() const;
	QString content() const;
	int crossref() const;

private:
	bool mIndent = false;
	GuiDblpXmlLineType::Type mType;
	QString mTag;
	QString mAttributes;
	QString mContent;
	int mCrossref = -1;
};
#endif // GUI_DBLP_XML_LINE_H
