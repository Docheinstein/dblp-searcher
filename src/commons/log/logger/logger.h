#ifndef LOGGER_H
#define LOGGER_H

#include <QDebug>
#include "commons/log/config/log_config.h"
#include "commons/globals/globals.h"


#define PRINT_TRACE_REAL(out)	qDebug().noquote().nospace() << "[T] " << out
#define PRINT_DEBUG_REAL(out)	qDebug().noquote().nospace() << "[D] " << out
#define PRINT_VERBOSE_REAL(out) qDebug().noquote().nospace() << "[V] " << out
#define PRINT_INFO_REAL(out)	qInfo().noquote().nospace() << "[I] " << out
#define PRINT_WARN_REAL(out)	qWarning().noquote().nospace() << "[W] " << out
#define PRINT_ERROR_REAL(out)	qCritical().noquote().nospace() << "[E] " << out

#define PRINT_TRACE(tag, msg)	PRINT_TRACE_REAL("{" << tag << "} " << msg)
#define PRINT_DEBUG(tag, msg)	PRINT_DEBUG_REAL("{" << tag << "} " << msg)
#define PRINT_VERBOSE(tag, msg) PRINT_VERBOSE_REAL("{" << tag << "} " << msg)
#define PRINT_INFO(tag, msg)	PRINT_INFO_REAL("{" << tag << "} " << msg)
#define PRINT_WARN(tag, msg)	PRINT_WARN_REAL("{" << tag << "} " << msg)
#define PRINT_ERROR(tag, msg)	PRINT_ERROR_REAL("{" << tag << "} " << msg)

#if TRACE_GLOBAL
#define TRACERT(x) "(" << __FILENAME__ << ":" << __LINE__ << " @ " << __func__ << ") " << x
#else
#define TRACERT(x) x
#endif

#if TRACE
#define _tt(x) if (CAN_LOG) PRINT_TRACE(LOG_TAG, TRACERT(x))
#define tt(x) if (canLog()) PRINT_TRACE(logTag(), TRACERT(x))
#else
#define _tt(x)
#define tt(x)
#endif

#if DEBUG
#define _dd(x)  if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT(x))
#define _dd1(x) if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT("| " << x))
#define _dd2(x) if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT("| | " << x))
#define _dd3(x) if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT("| | | " << x))
#define _dd4(x) if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT("| | | | " << x))
#define _dd5(x) if (CAN_LOG) PRINT_DEBUG(LOG_TAG, TRACERT("| | | | | " << x))

#define dd(x)  if (canLog()) PRINT_DEBUG(logTag(), TRACERT(x))
#define dd1(x) if (canLog()) PRINT_DEBUG(logTag(), TRACERT("| " << x))
#define dd2(x) if (canLog()) PRINT_DEBUG(logTag(), TRACERT("| | " << x))
#define dd3(x) if (canLog()) PRINT_DEBUG(logTag(), TRACERT("| | | " << x))
#define dd4(x) if (canLog()) PRINT_DEBUG(logTag(), TRACERT("| | | | " << x))
#define dd5(x) if (canLog()) PRINT_DEBUG(logTag(), TRACERT("| | | | | " << x))
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
#define _vv(x)  if (CAN_LOG) PRINT_VERBOSE(LOG_TAG, TRACERT(x))
#define _vv1(x) if (CAN_LOG) PRINT_VERBOSE(LOG_TAG, TRACERT("| " << x))
#define _vv2(x) if (CAN_LOG) PRINT_VERBOSE(LOG_TAG, TRACERT("| | " << x))
#define _vv3(x) if (CAN_LOG) PRINT_VERBOSE(LOG_TAG, TRACERT("| | | " << x))
#define _vv4(x) if (CAN_LOG) PRINT_VERBOSE(LOG_TAG, TRACERT("| | | | " << x))

#define vv(x)  if (canLog()) PRINT_VERBOSE(logTag(), TRACERT(x))
#define vv1(x) if (canLog()) PRINT_VERBOSE(logTag(), TRACERT("| " << x))
#define vv2(x) if (canLog()) PRINT_VERBOSE(logTag(), TRACERT("| | " << x))
#define vv3(x) if (canLog()) PRINT_VERBOSE(logTag(), TRACERT("| | | " << x))
#define vv4(x) if (canLog()) PRINT_VERBOSE(logTag(), TRACERT("| | | | " << x))
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
#define _ii_(x) PRINT_INFO_REAL(TRACERT(x))
#define _ii(x) if (CAN_LOG) PRINT_INFO(LOG_TAG, TRACERT(x))
#define ii(x) if (canLog()) PRINT_INFO(logTag(), TRACERT(x))
#else
#define _ii_(x)
#define _ii(x)
#define ii(x)
#endif

#if WARN
#define _ww_(x) PRINT_WARN_REAL(TRACERT(x))
#define _ww(x) if (CAN_LOG) PRINT_WARN(LOG_TAG, TRACERT(x))
#define ww(x) if (canLog()) PRINT_WARN(logTag(), TRACERT(x))
#else
#define _ww_(x)
#define _ww(x)
#define ww(x)
#endif

#if ERROR
#define _ee_(x) PRINT_ERROR_REAL(TRACERT(x))
#define _ee(x) if (CAN_LOG) PRINT_ERROR(LOG_TAG, TRACERT(x))
#define ee(x) if (canLog()) PRINT_ERROR(logTag(), TRACERT(x))
#else
#define _ee_(x)
#define _ee(x)
#define ee(x)
#endif

#endif // LOGGER_H
