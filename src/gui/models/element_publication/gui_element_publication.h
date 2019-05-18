#ifndef GUI_ELEMENT_PUBLICATION_H
#define GUI_ELEMENT_PUBLICATION_H

#include <QString>
#include "dblp/shared/defs/defs.h"

class GuiElementPublication {

public:
	class Builder {
	public:
		Builder & serial(elem_serial);
		GuiElementPublication build();

	private:
		elem_serial mSerial;
	};

	elem_serial serial() const;
	QString identifier() const;

private:
	elem_serial mSerial;
	QString mIdentifier;
};

#endif // GUI_ELEMENT_PUBLICATION_H
