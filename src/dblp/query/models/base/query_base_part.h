#ifndef QUERY_BASE_PART_H
#define QUERY_BASE_PART_H

#include <QString>
#include <QStringList>

class QueryBasePart {
public:
	virtual ~QueryBasePart() {}
	void addToken(const QString &token);

	QStringList tokens();
	virtual operator QString();

protected:
	QStringList mTokens;
};

#endif // QUERY_BASE_PART_H
