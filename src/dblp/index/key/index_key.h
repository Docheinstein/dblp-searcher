#ifndef INDEX_KEY_H
#define INDEX_KEY_H

#include <QString>
#include <QtGlobal>

struct IndexKey {
	QString key;
	qint64 originalFilePosition;
};

#endif // INDEX_KEY_H
