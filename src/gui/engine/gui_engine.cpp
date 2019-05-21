#include "gui_engine.h"

#include <QQmlEngine>

GuiEngine GuiEngine::instance(QQmlEngine *engine)
{
	static GuiEngine INSTANCE;

	// Set the engine, if this is the first time this has been called
	if (engine && !INSTANCE.mEngine)
		INSTANCE.mEngine = engine;

	return INSTANCE;
}

QQmlEngine *GuiEngine::engine() const
{
	return mEngine;
}
