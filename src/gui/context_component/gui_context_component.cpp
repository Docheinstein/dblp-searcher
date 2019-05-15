#include "gui_context_component.h"
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#include <QEventLoop>
#include "commons/util/util.h"
#include "gui/engine/gui_engine.h"

bool GuiContextComponent::create()
{
//	GuiEngine::instance().engine()->setObjectOwnership(this, ownership());

	QUrl qmlResource = Util::Qml::resourceUrl(qmlName());

	vv("Going to create GUI component; " <<
	   "resource: '" << qmlResource << "' | " <<
	   "context name: '" << contextName() << "'");

//	mContext = new QQmlContext(GuiEngine::instance().engine()->rootContext());
//	mContext->setContextProperty(qmlName(), this);

	GuiEngine::instance().engine()->rootContext()->setContextProperty(contextName(), this);

	mComponent = new QQmlComponent(GuiEngine::instance().engine(), qmlResource);

	bool createImmediately = true;

	if (!mComponent->isReady()) {
		dd1("Blocking thread until component creation");
		QEventLoop loop;

		// Detect when the component has been created
		QObject::connect(this, &GuiContextComponent::componentCreated,
						 &loop, &QEventLoop::quit);

		// Wait for QQmlComponent to load the qml file before create the component
		QObject::connect(mComponent, &QQmlComponent::statusChanged,
						 this, &GuiContextComponent::createComponent);

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

void GuiContextComponent::createComponent()
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
