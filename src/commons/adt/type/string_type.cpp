#include "string_type.h"

StringType::StringType(const char *string) : mString(string)
{

}

const char * StringType::string() const
{
	return mString;
}

//StringType::operator QString() const
//{
//	return mString;
//}

StringType::operator const char *()
{
	return mString;
}

