#include "gui_component.h"

#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QEventLoop>
#include "commons/util/util.h"
#include "gui/engine/gui_engine.h"

LOGGING(GuiComponent, true)

GuiComponent::~GuiComponent()
{
	delete mComponent;
	delete mContext;
}

bool GuiComponent::create()
{
	QUrl qmlResource = Util::Qml::resourceUrl(qmlResourceName());

	vv("Going to create GUI component; " <<
	   "resource: '" << qmlResource << "' | " <<
	   "name: '" << qmlName() << "'");

	mContext = new QQmlContext(GuiEngine::instance().engine()->rootContext());
	mContext->setContextProperty(qmlName(), this);

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

		// Double check, just in case...
		if (!mComponent->isReady()) {
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
		vv("Actually creating component '" << qmlResourceName() << "'");
		mComponent->create(mContext);
		emit componentCreated();
	}
	else if (mComponent->isError()) {
		ee("Errors occurred while creating component '" << qmlResourceName() << "'");
		ee("Reason: " << mComponent->errorString());
		emit componentCreated();
	}
}


