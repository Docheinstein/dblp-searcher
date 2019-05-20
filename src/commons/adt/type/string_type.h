#ifndef STRING_TYPE_H
#define STRING_TYPE_H

#include <QString>

class StringType {
public:
	QString string() const;
	operator QString() const;

protected:
	StringType(const QString &string);
	QString mString;
};

#endif // STRING_TYPE_H
