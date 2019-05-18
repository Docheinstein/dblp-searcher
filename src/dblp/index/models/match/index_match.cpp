#include "index_match.h"
#include "commons/globals/globals.h"

//uint qHash(const IndexMatch &efm)
//{
//	return ((efm.elementSerial << efm.fieldNumber) ^ efm.matchPosition) ^ UINT32(efm.fieldType);
//}

IndexMatch::operator QString() const
{
	QString s;
	s += "-- index match --\n";
	s += "Serial: " + DEC(elementSerial) + "\n";
	s += "Match pos: " + DEC(matchPosition) + "\n";
	s += "Field num: " + DEC(fieldNumber) + "\n";
	s += "Field type: " + elementFieldTypeString(fieldType) + "\n";

	int i = 1;
	for (const QString &matchedToken : matchedTokens) {
		s += DEC(i) + ". matched token" + matchedToken + "\n";
		i++;
	}

	return s;
}
