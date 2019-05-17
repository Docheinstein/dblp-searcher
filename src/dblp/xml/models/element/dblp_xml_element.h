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

#endif // DBLP_XML_ELEMENT_H
