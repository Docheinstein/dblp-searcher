#include "logger.h"

QDebug Logger::verbose(const char *tag)
{
	return qDebug().noquote().nospace() << "[V] {" << tag << "} ";
}

QDebug Logger::debug(const char *tag)
{
	return qDebug().noquote().nospace() << "[D] {" << tag << "} ";
}

QDebug Logger::info(const char *tag)
{
	return qInfo().noquote().nospace() << "[I] {" << tag << "} ";
}

QDebug Logger::warn(const char *tag)
{
	return qWarning().noquote().nospace() << "[W] {" << tag << "} ";
}

QDebug Logger::error(const char *tag)
{
	return qCritical().noquote().nospace() << "[E] {" << tag << "} ";
}
