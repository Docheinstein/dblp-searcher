#include "index_match.h"
#include "commons/globals/globals.h"

IndexMatch::operator QString() const
{
	QString s;

	s += "-- INDEX MATCH --\n";
	s += "Serial: " + DEC(elementSerial) + "\n";
	s += "Match pos: " + DEC(matchPosition) + "\n";
	s += "Field num: " + DEC(fieldNumber) + "\n";
	s += "Field type: " + elementFieldTypeString(fieldType) + "\n";

	int i = 1;

	for (const QString &matchedToken : matchedTokens) {
		s += DEC(i) + ". matched token: " + matchedToken + "\n";
		++i;
	}

	return s;
}

bool operator==(const IndexMatch &efm1, const IndexMatch &efm2) {
	return
			efm1.elementSerial == efm2.elementSerial &&
			efm1.fieldType == efm2.fieldType &&
			efm1.fieldNumber == efm2.fieldNumber &&
			efm1.matchPosition == efm2.matchPosition;
}

uint qHash(const IndexMatch &efm) {
	static quint32 SERIAL_ENLARGER =
			UINT_MAX / Config::Index::PostingList::ELEMENT_SERIAL_THRESHOLD;

	return qHash(efm.elementSerial * SERIAL_ENLARGER) ^
			UINT32(((17506 + efm.fieldNumber) * 0x45d9f3b) << 16) ^
			UINT32(((8868 + efm.matchPosition) * 0x44c923b) << 8) ^
			UINT32(efm.fieldType);
}
