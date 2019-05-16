#ifndef GUI_ELEMENT_DETAILS_H
#define GUI_ELEMENT_DETAILS_H

#define GUI_ELEMENT_DETAILS_QML_NAME "ElementDetails"

#include "dblp/shared/defs/defs.h"
#include "commons/log/loggable/loggable.h"
#include "gui/models/dblp_xml_lines/gui_dblp_xml_lines.h"

#include <QFutureWatcher>
#include <QObject>
#include <QString>

class GuiElementDetails : public QObject, Loggable {
	Q_OBJECT

	Q_PROPERTY(int serial READ serial WRITE setSerial NOTIFY serialChanged)
	Q_PROPERTY(QObject * xmlLines READ xmlLines NOTIFY xmlLinesChanged)

public:
	GuiElementDetails();
	int serial();
	void setSerial(int serial);

	QObject * xmlLines();

public slots:
	void xmlLoadingFinished();

signals:
	void serialChanged();
	void xmlLinesChanged();

protected:
	LOGGING_OVERRIDE

private:
	void loadXml();

	elem_serial mSerial;

	QVector<GuiDblpXmlLine> mLinesRaw;
	GuiDblpXmlLines mLines;

	QFutureWatcher<void> mXmlLoadingWatcher;
};

#endif // GUI_ELEMENT_DETAILS_H
