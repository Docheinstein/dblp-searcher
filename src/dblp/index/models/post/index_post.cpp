#include "index_post.h"

#include <QString>
#include <QHash>
#include "commons/globals/globals.h"

IndexPost::operator QString() const
{
	return QString("{") +
		"element_id = " + DEC(elementSerial)  + " | " +
		"field_num = " + DEC(fieldNumber)  + " | " +
		"in_field_pos = " + DEC(inFieldTermPosition) +
	"}";
}

bool operator==(const IndexPost &ip1, const IndexPost &ip2)
{
	return ip1.elementSerial == ip2.elementSerial &&
			ip1.fieldNumber == ip2.fieldNumber &&
			ip1.inFieldTermPosition == ip2.inFieldTermPosition;
}

uint qHash(const IndexPost &ip, uint seed)
{
	return qHash(ip.elementSerial, seed) ^
			ip.fieldNumber * ip.inFieldTermPosition;
}
