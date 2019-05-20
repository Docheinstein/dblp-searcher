#ifndef TEXT_STREAM_FILE_H
#define TEXT_STREAM_FILE_H

#include <QTextStream>
#include "commons/file/filestream/base/stream_file.h"

class TextStreamFile : public StreamFile {
public:
	bool open(const QString &path, QFile::OpenMode openMode) override;
	bool openWrite(const QString &path, bool append = false) override;
	bool openRead(const QString &path) override;

	QTextStream stream;
};

#endif // TEXT_STREAM_FILE_H
