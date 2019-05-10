#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>

class Logger
{
public:
	static QDebug trace(const char *tag);
	static QDebug debug(const char *tag);
	static QDebug verbose(const char *tag);
	static QDebug info(const char *tag);
	static QDebug warn(const char *tag);
	static QDebug error(const char *tag);
};

#endif // LOGGER_H
