#ifndef LOG_H
#define LOG_H

#include <QString>
#include <QDebug>

#define VERBOSE 0
#define DEBUG 0
#define INFO 1
#define WARN 1
#define ERROR 1

#if VERBOSE
#define vv(x)  if (L.isEnabled()) L.verbose() << x
#define vv1(x) if (L.isEnabled()) L.verbose() << "| " << x
#define vv2(x) if (L.isEnabled()) L.verbose() << "| | " << x
#define vv3(x) if (L.isEnabled()) L.verbose() << "| | | " << x
#define vv4(x) if (L.isEnabled()) L.verbose() << "| | | | " << x
#else
#define vv(x)
#define vv1(x)
#define vv2(x)
#define vv3(x)
#define vv4(x)
#endif

#if DEBUG
#define dd(x)  if (L.isEnabled()) L.debug() << x
#define dd1(x) if (L.isEnabled()) L.debug() << "| " << x
#define dd2(x) if (L.isEnabled()) L.debug() << "| | " << x
#define dd3(x) if (L.isEnabled()) L.debug() << "| | | " << x
#define dd4(x) if (L.isEnabled()) L.debug() << "| | | | " << x
#else
#define dd(x)
#define dd1(x)
#define dd2(x)
#define dd3(x)
#define dd4(x)
#endif

#if INFO
#define ii(x) if (L.isEnabled()) L.info() << x
#else
#define ii(x)
#endif

#if WARN
#define ww(x) if (L.isEnabled()) L.warn() << x
#else
#define ww(x)
#endif

#if ERROR
#define ee(x) if (L.isEnabled()) L.error() << x
#else
#define ee(x)
#endif

class Logger
{
public:
	static Logger forTag(QString tag, bool enabled = true);
	static Logger forClass(QString className, bool enabled = true);

	QDebug verbose();
	QDebug debug();
	QDebug info();
	QDebug warn();
	QDebug error();

	bool isEnabled();

private:
	Logger(QString tag, bool enabled);

	QString mTag;
	bool mEnabled;
};

#endif // LOG_H
