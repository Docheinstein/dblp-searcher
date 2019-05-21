#ifndef GUI_ELEMENT_DETAILS_H
#define GUI_ELEMENT_DETAILS_H

#define GUI_ELEMENT_DETAILS_QML_NAME "ElementDetails"

#include <QString>

#include "commons/log/loggable/loggable.h"
#include "dblp/xml/models/element/dblp_xml_element.h"
#include "dblp/xml/handler/dblp_xml_parse_handler.h"
#include "gui/models/dblp_xml_lines/gui_dblp_xml_lines.h"
#include "gui/models/element_publications/gui_element_publications.h"



// XML Retriever stuff BEGIN

class DblpElementXmlRetrieverHandler : public QObject {
	Q_OBJECT

public:
	virtual ~DblpElementXmlRetrieverHandler();
	virtual void onElementRetrieved(const DblpXmlElement &elem) = 0;

signals:
	void elementRetrieved();
};

class DblpElementXmlRetriever : public DblpXmlParseHandler {
public:
	DblpElementXmlRetriever(elem_serial serial, DblpElementXmlRetrieverHandler &handler);
	void onParseStart() override;
	void onParseEnd() override;
	bool onElement(const DblpXmlElement &element, qint64 pos) override;

private:
	elem_serial mSerial;
	QString mIdentifier;
	DblpElementXmlRetrieverHandler &mHandler;
};

// XML Retriever stuff END

class GuiElementDetails : public DblpElementXmlRetrieverHandler, protected Loggable {
	Q_OBJECT

	Q_PROPERTY(int serial READ serial WRITE setSerial NOTIFY serialChanged)
//	Q_PROPERTY(int crossrefSerial READ crossrefSerial NOTIFY crossrefSerialChanged)
	Q_PROPERTY(QString identifier READ identifier NOTIFY identifierChanged)
	Q_PROPERTY(QObject * xmlLines READ xmlLines NOTIFY xmlLinesChanged)
	Q_PROPERTY(QObject * publications READ publications NOTIFY publicationsChanged)
	Q_PROPERTY(int publicationsCount READ publicationsCount NOTIFY publicationsCountChanged)

	Q_PROPERTY(bool hasXml READ hasXml NOTIFY hasXmlChanged)
	Q_PROPERTY(bool hasPublications READ hasPublications NOTIFY hasPublicationsChanged)

public:
	~GuiElementDetails() override;
	GuiElementDetails();

	int serial() const;
	void setSerial(int serial);

//	int crossrefSerial();

	QString identifier() const;

	QObject * xmlLines();

	QObject * publications();

	int publicationsCount() const;

	bool hasXml() const;
	bool hasPublications() const;

	void onElementRetrieved(const DblpXmlElement &elem) override;

public slots:
	void xmlLoadingFinished();

signals:
	void serialChanged();
//	void crossrefSerialChanged();
	void identifierChanged();
	void xmlLinesChanged();
	void publicationsChanged();
	void publicationsCountChanged();
	void hasXmlChanged();
	void hasPublicationsChanged();

protected:
	LOGGING_OVERRIDE

private:
	elem_serial mSerial = UINT_MAX;
	bool mHasCrossref;
	elem_serial mCrossrefSerial;
	QString mIdentifier;

	bool mHasXml = false;
	QVector<GuiDblpXmlLine> mLinesRaw; // Temporary, before push to mLines
	GuiDblpXmlLines mLines;

	bool mHasPublications = false;
	QVector<GuiElementPublication> mPublicationsRaw; // Temporary, before push to mPublications
	GuiElementPublications mPublications;

	void init();
	void loadXml();
	void handleXmlRetrived(const DblpXmlElement &element);

};

#endif // GUI_ELEMENT_DETAILS_H
