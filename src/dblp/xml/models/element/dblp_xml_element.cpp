#include "dblp_xml_element.h"

DblpXmlElement::operator QString() const
{
	QString s;
	s += "Name: " + name + "\n";

	for (auto it = attributes.cbegin(); it != attributes.cend(); it++) {
		s+= "Attr (" + it->name() + " : " + it->value() + ")" + "\n";
	}

	for (auto it = fields.cbegin(); it != fields.cend(); it++) {
		const QVector<QString> fields = it.value();
		for (const QString &field : fields) {
				s+= "Field (" + it.key() + " : " + field + ")" + "\n";
		}
	}

	return s;
}
