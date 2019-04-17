#ifndef DBLP_XML_PARSE_HANDLER_H
#define DBLP_XML_PARSE_HANDLER_H

#include "dblp/entities/dblp_entities.h"

class DblpXmlParseHandler
{
public:
	virtual ~DblpXmlParseHandler();

	virtual void onStart() = 0;
	virtual void onEnd() = 0;

	virtual void onArticle(DblpArticle &article, qint64 pos) = 0;
	virtual void onJournal(DblpJournal &journal, qint64 pos) = 0;

	virtual void onIncollection(DblpIncollection &incollection, qint64 pos) = 0;
	virtual void onBook(DblpBook &book, qint64 pos) = 0;

	virtual void onInproceedings(DblpInproceedings &inproc, qint64 pos) = 0;
	virtual void onProceedings(DblpProceedings &proc, qint64 pos) = 0;

	virtual void onPhdThesis(DblpPhdThesis &phd, qint64 pos) = 0;
	virtual void onMasterThesis(DblpMasterThesis &master, qint64 pos) = 0;

};

#endif // DBLP_XML_PARSE_HANDLER_H
