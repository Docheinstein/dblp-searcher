#ifndef IR_MODEL_H
#define IR_MODEL_H

#include <dblp/index/handler/index_handler.h>
#include <dblp/query/query/query.h>


class IRModel {
public:
	IRModel(IndexHandler *indexHandler);

	float ief(const QString &term);

	IndexHandler * index();

private:
	static Logger L;

	void init();
	void computeIefs();
	void debug_printIefs();

	QHash<QString, float> mIefs;	// := inverse element frequency
									// log10(|E|/ef_t)
	IndexHandler *mIndex;
};

#endif // IR_MODEL_H
