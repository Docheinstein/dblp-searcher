#include "string_type.h"

StringType::StringType(const QString &string) : mString(string) {}

QString StringType::string() const
{
	return mString;
}

StringType::operator QString() const
{
	return mString;
}
