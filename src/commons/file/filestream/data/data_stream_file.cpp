#include "data_stream_file.h"

bool DataStreamFile::open(const QString &path, QIODevice::OpenMode openMode)
{
	if (!StreamFile::open(path, openMode))
		return false;

	stream.setDevice(&file);
	return true;
}

bool DataStreamFile::openWrite(const QString &path, bool append)
{
	if (!StreamFile::openWrite(path, append))
		return false;

	stream.setDevice(&file);
	return true;
}

bool DataStreamFile::openRead(const QString &path)
{
	if (!StreamFile::openRead(path))
		return false;

	stream.setDevice(&file);
	return true;
}

void DataStreamFileMutex::lock()
{
	mutex.lock();
}

void DataStreamFileMutex::unlock()
{
	mutex.unlock();
}
