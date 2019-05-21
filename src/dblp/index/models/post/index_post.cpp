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

uint qHash(const IndexPost &ip)
{
	static quint32 SERIAL_ENLARGER =
			UINT_MAX / Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD;

	return qHash(ip.elementSerial * SERIAL_ENLARGER) ^
			UINT32(((1706 + ip.fieldNumber) * 0x13dff3d) << 16) ^
			UINT32(((245 + ip.inFieldTermPosition) * 0x42c9231) << 8);
}
