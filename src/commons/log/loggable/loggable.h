#ifndef LOGGABLE_H
#define LOGGABLE_H

#include <QDebug>
#include "commons/log/logger/logger.h"

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

class Loggable
{
protected:
	virtual ~Loggable();
	virtual const char * logTag() const = 0;
	virtual bool canLog() const = 0;
};

#endif // LOGGABLE_H
