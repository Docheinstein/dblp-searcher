#include "gui_component.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QEventLoop>

#include "commons/util/util.h"
#include "gui/engine/gui_engine.h"

LOGGING(GuiComponent, true)

GuiComponent::~GuiComponent() {}

bool GuiComponent::create()
{
	QUrl qmlResource = Util::Resources::qmlUrl(qmlName());

	vv("Going to create GUI component; resource: '" << qmlResource);

	mComponent = new QQmlComponent(GuiEngine::instance().engine(), qmlResource);

	bool createImmediately = true;

	if (!mComponent->isReady()) {
		dd1("Blocking thread until component creation");
		QEventLoop loop;

		// Detect when the component has been created
		QObject::connect(this, &GuiComponent::componentCreated,
						 &loop, &QEventLoop::quit);

		// Wait for QQmlComponent to load the qml file before create the component
		QObject::connect(mComponent, &QQmlComponent::statusChanged,
						 this, &GuiComponent::createComponent);

		if (mComponent->isError()) {
			ee("Errors occurred while creating component '" << qmlName() << "'");
			ee("Reason: " << mComponent->errorString());
			emit componentCreated();
		}
		// Double check, just in case...
		else if (!mComponent->isReady()) {
			createImmediately = false;
			// Wait until the component has been created
			loop.exec();
		}
	}

	if (createImmediately) {
		dd1("Creating component immediately");
		// Create immediately
		createComponent();
	}

	return !mComponent->isError();
}

void GuiComponent::createComponent()
{
	if (mComponent->isReady()) {
		vv("Actually creating component '" << qmlName() << "'");
		mComponent->create();
		emit componentCreated();
	}
	else if (mComponent->isError()) {
		ee("Errors occurred while creating component '" << qmlName() << "'");
		ee("Reason: " << mComponent->errorString());
		emit componentCreated();
	}
}


bool GuiComponent::shown() const
{
	return mShown;
}

void GuiComponent::setShown(bool shown)
{
	mShown = shown;
	emit shownChanged();
}

