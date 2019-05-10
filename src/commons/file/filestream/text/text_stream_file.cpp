#include "text_stream_file.h"

bool TextStreamFile::open(const QString &path, QIODevice::OpenMode openMode)
{
	if (!StreamFile::open(path, openMode))
		return false;

	stream.setDevice(&file);
	return true;
}

bool TextStreamFile::openWrite(const QString &path, bool append)
{
	if (!StreamFile::openWrite(path, append))
		return false;

	stream.setDevice(&file);
	return true;
}

bool TextStreamFile::openRead(const QString &path)
{
	if (!StreamFile::openRead(path))
		return false;

	stream.setDevice(&file);
	return true;
}
