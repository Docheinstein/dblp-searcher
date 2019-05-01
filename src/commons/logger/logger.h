#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QDebug>

#define VERBOSE 1
#define DEBUG 1
#define INFO 1
#define WARN 1
#define ERROR 1

#if VERBOSE
#define vv(x) L.verbose() << x
#else
#define vv(x)
#endif

#if DEBUG
#define dd(x) L.debug() << x
#else
#define dd(x)
#endif

#if INFO
#define ii(x) L.info() << x
#else
#define ii(x)
#endif

#if WARN
#define ww(x) L.warn() << x
#else
#define ww(x)
#endif

#if ERROR
#define ee(x) L.error() << x
#else
#define ee(x)
#endif

class Logger
{
public:
	static Logger forTag(QString tag);
	static Logger forClass(QString className);

	QDebug verbose();
	QDebug debug();
	QDebug info();
	QDebug warn();
	QDebug error();

private:
	Logger(QString tag);
	QString mTag;
};

#endif // LOG_H
