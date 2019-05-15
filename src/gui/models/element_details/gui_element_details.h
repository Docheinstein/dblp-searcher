#ifndef GUI_ELEMENT_DETAILS_H
#define GUI_ELEMENT_DETAILS_H

#define GUI_ELEMENT_DETAILS_QML_NAME "ElementDetails"

#include "dblp/shared/defs/defs.h"
#include "commons/log/loggable/loggable.h"

#include <QObject>
#include <QString>

class GuiElementDetails : public QObject, Loggable {
	Q_OBJECT

	Q_PROPERTY(int elementSerial READ elementSerial WRITE setElementSerial NOTIFY elementSerialChanged)
	Q_PROPERTY(QString elementXml READ elementXml NOTIFY elementXmlChanged)

public:
	int elementSerial();
	void setElementSerial(int serial);

	QString elementXml();

signals:
	void elementSerialChanged();
	void elementXmlChanged();

protected:
	LOGGING_OVERRIDE

private:
	void setElementXml(const QString &xml);

	elem_serial mElementSerial;
	QString mElementXml;

};

#endif // GUI_ELEMENT_DETAILS_H
