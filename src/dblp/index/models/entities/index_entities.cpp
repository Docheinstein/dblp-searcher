#include "index_entities.h"
#include <QDebug>

QDebug operator<<(QDebug stream, const DblpWork &entity)
{
	QDebugStateSaver saver(stream);
	stream <<
		"(key: " << entity.key << ", " <<
		"title: " << entity.title << ", " <<
		"year: " << entity.year << ")";

	return stream;
}

QDebug operator<<(QDebug stream, const DblpPublication &entity)
{
	QDebugStateSaver saver(stream);
	stream <<
		"(key: " << entity.key << ", " <<
		"author: " << (entity.authors.empty() ? "" : entity.authors[0]) << ", " <<
		"title: " << entity.title << ", " <<
		"year: " << entity.year << ")";

	return stream;
}
