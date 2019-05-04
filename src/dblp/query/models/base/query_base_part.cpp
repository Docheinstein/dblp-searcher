#include "query_base_part.h"
#include "commons/util/util.h"

void QueryBasePart::addToken(const QString &token)
{
	// Sanitize the token before append it
	mTokens.append(Util::String::sanitizeTerm(token));
}


QStringList QueryBasePart::tokens()
{
	return mTokens;
}

QueryBasePart::operator QString()
{
	return "TOKENS: {" + mTokens.join(", ") + "}";
}
