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
		i++;
	}

	return s;
}
