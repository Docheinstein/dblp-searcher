#include "logger.h"

#include <QDebug>

Logger::Logger(QString tag)
{
	mTag = tag;
}

Logger Logger::forTag(QString tag)
{
	return Logger(tag);
}

Logger Logger::forClass(QString className)
{
	return Logger("{" + className + "}");
}

QDebug Logger::verbose()
{
	return qDebug().noquote().nospace() << "[V] " << mTag << " ";
}

QDebug Logger::debug()
{
	return qDebug().noquote().nospace() << "[D] " << mTag << " ";
}

QDebug Logger::info()
{
	return qInfo().noquote().nospace() << "[I] " << mTag << " ";
}

QDebug Logger::warn()
{
	return qWarning().noquote().nospace() << "[W] " << mTag << " ";
}

QDebug Logger::error()
{
	return qCritical().noquote().nospace() << "[E] " << mTag << " ";
}
