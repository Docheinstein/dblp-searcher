#ifndef DBLP_XML_ELEMENT_H
#define DBLP_XML_ELEMENT_H

#include <QMap>
#include <QString>
#include <QXmlStreamAttributes>

struct DblpXmlElement {
	QString name;
	QXmlStreamAttributes attributes;
	QMap<QString, QVector<QString>> fields;

	operator QString() const;
};


// Needed for use DblpXmlElement with the Qt signals/slots paradigm
Q_DECLARE_METATYPE(DblpXmlElement);

#endif // DBLP_XML_ELEMENT_H
