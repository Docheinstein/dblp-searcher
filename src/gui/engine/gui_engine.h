#ifndef GUI_ENGINE_H
#define GUI_ENGINE_H

class QQmlEngine;

class GuiEngine {
public:
	static GuiEngine instance(QQmlEngine *engine = nullptr);

	QQmlEngine *engine();

private:
	QQmlEngine *mEngine;
};

#endif // GUI_ENGINE_H
