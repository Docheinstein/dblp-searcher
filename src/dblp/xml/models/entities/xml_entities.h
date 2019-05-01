#ifndef DBLP_XML_ENTITIES_H
#define DBLP_XML_ENTITIES_H

#include <QString>
#include <QList>

// Base entities

class DblpBase {
public:
	QString key;
};

class DblpWork : public DblpBase {
public:
	QString title;
	QString year;
};

class DblpPublication : public DblpWork {
public:
	QList<QString> authors;
};

class DblpVenue : public DblpWork {
public:
	QString publisher;
};

// Entities

class DblpArticle : public DblpPublication {
public:
	QString journal; // -> journal
};

// Actually doesn't exists in dblp.xml,
// but comes from articles' crossrefs
class DblpJournal {
public:
	QString name;
};

class DblpIncollection : public DblpPublication {
public:
	QString booktitle;
//	QString crossref; // -> book
};

class DblpBook : public DblpVenue {
public:
	QList<QString> authors;	// Should we index authors, since this is a venue?
};

class DblpInproceedings : public DblpPublication {
public:
	QString booktitle;
//	QString crossref; // -> proceedings
};

class DblpProceedings : public DblpVenue {
public:
	QString booktitle;
};

class DblpPhdThesis : public DblpPublication {};
class DblpMasterThesis : public DblpPublication {};

//class DblpPerson : public

QDebug operator<<(QDebug stream, const DblpWork &entity);
QDebug operator<<(QDebug stream, const DblpPublication &entity);

#endif // DBLP_XML_ENTITIES_H
