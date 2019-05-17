#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include <QString>

#define STRING_TYPE
class StringType {
public:
	const char * string() const;
	operator const char *();

protected:
	StringType(const char * string);
	const char * mString;
};

#endif // STRING_TYPE_H
