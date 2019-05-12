#ifndef INDEX_POST_H
#define INDEX_POST_H

#include <QtGlobal>
#include <QHash>
#include "dblp/shared/defs/defs.h"
#include "commons/config/config.h"
#include "commons/globals/globals.h"
#include <limits.h>

struct IndexPost {
	elem_serial elementSerial;	// Serial number associated to the element
								// (that refers to a single identifier)

	field_num fieldNumber;	// Starting from 0, number of the field between
							// the fields with the same name in the element.
							// e.g. for distinguish between <author> tags

	term_pos inFieldTermPosition;	// Position of the term inside the field

	operator QString() const;
};


inline bool operator==(const IndexPost &ip1, const IndexPost &ip2)
{
	return ip1.elementSerial == ip2.elementSerial &&
			ip1.fieldNumber == ip2.fieldNumber &&
			ip1.inFieldTermPosition == ip2.inFieldTermPosition;
}

inline uint qHash(const IndexPost &ip)
{
	static quint32 SERIAL_ENLARGER =
			UINT_MAX / Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD;

	return qHash(ip.elementSerial * SERIAL_ENLARGER) ^
			UINT32(((1706 + ip.fieldNumber) * 0x13dff3d) << 16) ^
			UINT32(((245 + ip.inFieldTermPosition) * 0x42c9231) << 8);
}
#endif // INDEX_POST_H
