#ifndef INDEX_MATCH_H
#define INDEX_MATCH_H

#include <QStringList>
#include "dblp/shared/defs/defs.h"
#include "dblp/shared/element_field_type/element_field_type.h"
#include "commons/globals/globals.h"
#include "commons/config/config.h"

typedef struct IndexMatch {
	// Element serial
	elem_serial elementSerial;

	// Where the tokens matches within the fieldType + fieldNumber.
	// In case of phrases, is the position of the first token that matches
	term_pos matchPosition;

	// Number of field of the same fieldType (e.g. for multiple author)
	field_num fieldNumber;

	// Field type
	ElementFieldType fieldType;

	// Token that matches (the given query) within this elementSerial+fieldType+fieldNumber
	QStringList matchedTokens;

	operator QString() const;
} IndexMatch;

bool operator==(const IndexMatch &efm1, const IndexMatch &efm2);
uint qHash(const IndexMatch &efm);


#endif // INDEX_MATCH_H
