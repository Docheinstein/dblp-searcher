#ifndef STREAM_FILE_H
#define STREAM_FILE_H

#include <QFile>

class StreamFile {
public:
	virtual ~StreamFile();
	virtual bool open(const QString &path, QFile::OpenMode openMode);
	virtual bool openWrite(const QString &path, bool append = false);
	virtual bool openRead(const QString &path);
	virtual bool close();

	qint64 fileSize();
	qint64 filePosition();

	QFile file;
};

#endif // STREAM_FILE_H
