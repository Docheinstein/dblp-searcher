#ifndef LOADER_H
#define LOADER_H

#include <QThread>

class Loader : public QThread {
	Q_OBJECT

public:
	Loader(const QString &indexPath,
		   const QString &baseIndexName);

signals:
	void loadFinished();

protected:
	void run() override;

	QString mIndexPath;
	QString mBaseIndexName;
};

#endif // LOADER_H
