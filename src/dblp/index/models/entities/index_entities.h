#ifndef INDEX_ENTITIES_H
#define INDEX_ENTITIES_H

#include <QString>
#include <QList>

// Base entities

struct DblpBase {
	QString key;
};

struct DblpWork :  DblpBase {
	QString title;
	QString year;
};

struct DblpPublication :  DblpWork {
	QVector<QString> authors;
};

struct DblpPublicationCrossref : DblpPublication {
	QString crossref;
};

struct DblpVenue : DblpWork {
	QString publisher;
};

// Entities

struct DblpArticle : DblpPublication {
	QString journal; // -> journal
};

// Actually doesn't exists in dblp.xml,
// but comes from articles' crossrefs
struct DblpJournal {
	QString name;
};

struct DblpIncollection :  DblpPublicationCrossref {
	QString booktitle;
}; 	// -> book

struct DblpBook :  DblpVenue {
	QVector<QString> authors;
};

struct DblpInproceedings :  DblpPublicationCrossref {
	QString booktitle;
}; // -> proceedings

struct DblpProceedings :  DblpVenue {};

struct DblpPhdThesis :  DblpPublication {};
struct DblpMasterThesis :  DblpPublication {};

//struct DblpPerson :

QDebug operator<<(QDebug stream, const DblpWork &entity);
QDebug operator<<(QDebug stream, const DblpPublication &entity);

#endif // INDEX_ENTITIES_H
