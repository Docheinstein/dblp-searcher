#ifndef INDEX_ENTITIES_H
#define INDEX_ENTITIES_H

#include <QString>
#include <QVector>

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

// |_ Publications

struct DblpArticle : DblpPublication {
	QString journal; // -> journal
};

struct DblpIncollection :  DblpPublicationCrossref {
	QString booktitle;
}; 	// -> book


struct DblpInproceedings :  DblpPublicationCrossref {
	QString booktitle;
}; // -> proceedings


struct DblpPhdThesis :  DblpPublication {};
struct DblpMastersThesis :  DblpPublication {};

// |_ Venues

// Actually doesn't exists in dblp.xml,
// but comes from articles' crossrefs
struct DblpJournal {
	QString name;
};

struct DblpBook :  DblpVenue {
	QVector<QString> authors;
};

struct DblpProceedings :  DblpVenue {};

QDebug operator<<(QDebug stream, const DblpWork &entity);
QDebug operator<<(QDebug stream, const DblpPublication &entity);

#endif // INDEX_ENTITIES_H
