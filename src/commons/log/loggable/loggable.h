#ifndef LOGGABLE_H
#define LOGGABLE_H

#include "commons/log/logger/logger.h"
#include "commons/globals/globals.h"

#define STATIC_LOGGING(clazz, enabled) \
	static bool CAN_LOG = enabled; \
	static const char * const LOG_TAG = #clazz; \

#define LOGGING_OVERRIDE \
	const char *logTag() const override; \
	bool canLog() const override;

#define LOGGING(clazz, enabled) \
	static bool CAN_LOG = enabled; \
	static const char * const LOG_TAG = #clazz; \
	const char * clazz::logTag() const { return LOG_TAG; } \
	bool clazz::canLog() const { return CAN_LOG; }

// Shut up every log level
#define SILENT_ALL			0
#define SILENT_ALMOST_ALL	0

// Active log levels (SILENT must be 0)
#define TRACE_GLOBAL	(1 && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define TRACE			(1 && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define DEBUG			(1 && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define VERBOSE			(1 && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define INFO			(1 && !SILENT_ALL)
#define WARN			(1 && !SILENT_ALL)
#define ERROR			(1 && !SILENT_ALL)


#if TRACE_GLOBAL
#define TRACERT(x) "(" << __FILENAME__ << ":" << __LINE__ << " @ " << __func__ << ") " << x
#else
#define TRACERT(x) x
#endif

#if TRACE
#define _tt(x) if (CAN_LOG) Logger::trace(LOG_TAG) << TRACERT(x)
#define tt(x) if (canLog()) Logger::trace(logTag()) << TRACERT(x)
#else
#define _tt(x)
#define tt(x)
#endif

#if DEBUG
#define _dd(x)  if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT(x)
#define _dd1(x) if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT("| " << x)
#define _dd2(x) if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT("| | " << x)
#define _dd3(x) if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT("| | | " << x)
#define _dd4(x) if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT("| | | | " << x)
#define _dd5(x) if (CAN_LOG) Logger::debug(LOG_TAG) << TRACERT("| | | | | " << x)

#define dd(x)  if (canLog()) Logger::debug(logTag()) << TRACERT(x)
#define dd1(x) if (canLog()) Logger::debug(logTag()) << TRACERT("| " << x)
#define dd2(x) if (canLog()) Logger::debug(logTag()) << TRACERT("| | " << x)
#define dd3(x) if (canLog()) Logger::debug(logTag()) << TRACERT("| | | " << x)
#define dd4(x) if (canLog()) Logger::debug(logTag()) << TRACERT("| | | | " << x)
#define dd5(x) if (canLog()) Logger::debug(logTag()) << TRACERT("| | | | | " << x)
#else
#define _dd(x)
#define _dd1(x)
#define _dd2(x)
#define _dd3(x)
#define _dd4(x)
#define _dd5(x)

#define dd(x)
#define dd1(x)
#define dd2(x)
#define dd3(x)
#define dd4(x)
#define dd5(x)
#endif

#if VERBOSE
#define _vv(x)  if (CAN_LOG) Logger::verbose(LOG_TAG) << TRACERT(x)
#define _vv1(x) if (CAN_LOG) Logger::verbose(LOG_TAG) << TRACERT("| " << x)
#define _vv2(x) if (CAN_LOG) Logger::verbose(LOG_TAG) << TRACERT("| | " << x)
#define _vv3(x) if (CAN_LOG) Logger::verbose(LOG_TAG) << TRACERT("| | | " << x)
#define _vv4(x) if (CAN_LOG) Logger::verbose(LOG_TAG) << TRACERT("| | | | " << x)

#define vv(x)  if (canLog()) Logger::verbose(logTag()) << TRACERT(x)
#define vv1(x) if (canLog()) Logger::verbose(logTag()) << TRACERT("| " << x)
#define vv2(x) if (canLog()) Logger::verbose(logTag()) << TRACERT("| | " << x)
#define vv3(x) if (canLog()) Logger::verbose(logTag()) << TRACERT("| | | " << x)
#define vv4(x) if (canLog()) Logger::verbose(logTag()) << TRACERT("| | | | " << x)
#else
#define _vv(x)
#define _vv1(x)
#define _vv2(x)
#define _vv3(x)
#define _vv4(x)

#define vv(x)
#define vv1(x)
#define vv2(x)
#define vv3(x)
#define vv4(x)
#endif


#if INFO
#define _ii(x) if (CAN_LOG) Logger::info(LOG_TAG) << TRACERT(x)
#define ii(x) if (canLog()) Logger::info(logTag()) << TRACERT(x)
#else
#define _ii(x)
#define ii(x)
#endif

#if WARN
#define _ww(x) if (CAN_LOG) Logger::warn(LOG_TAG) << TRACERT(x)
#define ww(x) if (canLog()) Logger::warn(logTag()) << TRACERT(x)
#else
#define _ww(x)
#define ww(x)
#endif

#if ERROR
#define _ee(x) if (CAN_LOG) Logger::error(LOG_TAG) << TRACERT(x)
#define ee(x) if (canLog()) Logger::error(logTag()) << TRACERT(x)
#else
#define _ee(x)
#define ee(x)
#endif

class Loggable
{
protected:
	virtual ~Loggable();
	virtual const char * logTag() const = 0;
	virtual bool canLog() const = 0;
};

#endif // LOG_H
