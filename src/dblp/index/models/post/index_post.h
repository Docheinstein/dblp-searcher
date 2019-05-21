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

bool operator==(const IndexPost &ip1, const IndexPost &ip2);
uint qHash(const IndexPost &ip);

#endif // INDEX_POST_H
