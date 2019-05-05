#include "logger.h"

#include <QDebug>

Logger::Logger(QString tag, bool enabled)
{
	mTag = tag;
	mEnabled = enabled;
}

Logger Logger::forTag(QString tag, bool enabled)
{
	return Logger(tag, enabled);
}

Logger Logger::forClass(QString className, bool enabled)
{
	return Logger("{" + className + "}", enabled);
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

bool Logger::isEnabled()
{
	return mEnabled;
}
