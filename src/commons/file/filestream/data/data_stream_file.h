#ifndef DATA_STREAM_FILE_H
#define DATA_STREAM_FILE_H

#include <QDataStream>
#include "commons/file/filestream/base/stream_file.h"

class DataStreamFile : public StreamFile {
public:
	bool open(const QString &path, QFile::OpenMode openMode) override;
	bool openWrite(const QString &path, bool append = false) override;
	bool openRead(const QString &path) override;

	QDataStream stream;
};

#endif // DATA_STREAM_FILE_H
