#include "stream_file.h"

StreamFile::~StreamFile()
{

}

bool StreamFile::open(const QString &path, QFile::OpenMode openMode)
{
	file.setFileName(path);
	return file.open(openMode);
}

bool StreamFile::openWrite(const QString &path, bool append)
{
	return open(path, QFile::WriteOnly | (append ? QFile::Append : QFile::Truncate));
}

bool StreamFile::openRead(const QString &path)
{
	return open(path, QFile::ReadOnly);
}

bool StreamFile::close()
{
	file.close();
	return true;
}

qint64 StreamFile::fileSize() const
{
	return file.size();
}

qint64 StreamFile::filePosition() const
{
	return file.pos();
}

