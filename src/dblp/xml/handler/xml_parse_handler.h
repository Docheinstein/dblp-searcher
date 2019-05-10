#ifndef DBLP_PARSE_HANDLER_H
#define DBLP_PARSE_HANDLER_H

#include "dblp/xml/models/entities/xml_entities.h"

class XmlParseHandler
{
public:
	virtual ~XmlParseHandler();

	virtual void onStart() = 0;
	virtual void onEnd() = 0;

	virtual void onArticle(const DblpArticle &article, qint64 pos) = 0;
	virtual void onJournal(const DblpJournal &journal, qint64 pos) = 0;

	virtual void onIncollection(const DblpIncollection &incollection, qint64 pos) = 0;
	virtual void onBook(const DblpBook &book, qint64 pos) = 0;

	virtual void onInproceedings(const DblpInproceedings &inproc, qint64 pos) = 0;
	virtual void onProceedings(const DblpProceedings &proc, qint64 pos) = 0;

	virtual void onPhdThesis(const DblpPhdThesis &phd, qint64 pos) = 0;
	virtual void onMasterThesis(const DblpMasterThesis &master, qint64 pos) = 0;
};

#endif // DBLP_PARSE_HANDLER_H
