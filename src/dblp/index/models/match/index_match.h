#ifndef INDEX_MATCH_H
#define INDEX_MATCH_H

#include <QStringList>
#include "dblp/shared/defs/defs.h"
#include "dblp/shared/element_field_type/element_field_type.h"
#include "commons/globals/globals.h"
#include "commons/config/config.h"

typedef struct IndexMatch {
//	IndexMatch() {}
	elem_serial elementSerial; // element id
	term_pos matchPosition; // where the tokens matches within
							// the fieldType + fieldNumber
							// starting position, in case of phrases
	field_num fieldNumber; // field number for the fieldType
	ElementFieldType fieldType;	// field type
	QStringList matchedTokens;

	operator QString() const;
} IndexMatch;

inline bool operator==(const IndexMatch &efm1, const IndexMatch &efm2) {
	return
		efm1.elementSerial == efm2.elementSerial &&
		efm1.fieldType == efm2.fieldType &&
		efm1.fieldNumber == efm2.fieldNumber &&
		efm1.matchPosition == efm2.matchPosition;
}

inline uint qHash(const IndexMatch &efm) {
	static quint32 SERIAL_ENLARGER =
			UINT_MAX / Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD;

	return qHash(efm.elementSerial * SERIAL_ENLARGER) ^
			UINT32(((17506 + efm.fieldNumber) * 0x45d9f3b) << 16) ^
			UINT32(((8868 + efm.matchPosition) * 0x44c923b) << 8) ^
			UINT32(efm.fieldType);
}


#endif // INDEX_MATCH_H
