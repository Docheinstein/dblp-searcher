#ifndef INDEX_POST_H
#define INDEX_POST_H

#include <QtGlobal>

struct IndexPost {
	quint32 elementId;			// Id associated to the element (that refers to a single key)

	quint32 fieldNumber;		// Starting from 0, number of the field between
							// the fields with the same name in the element.
							// e.g. for distinguish between <author> tags

	quint32 inFieldTermPosition;	// Position of the term inside the context
};

#endif // INDEX_POST_H
