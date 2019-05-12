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
