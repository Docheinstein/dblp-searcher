#ifndef IR_MODEL_H
#define IR_MODEL_H

#include <QString>

class IndexHandler;

class IRModel {
public:
	virtual ~IRModel();
	IRModel(IndexHandler *indexHandler);

	virtual float termScore(const QString &term) = 0;
	virtual float bonusFactorPerPhraseTerm();

	IndexHandler * index();

protected:
	IndexHandler *mIndex;
};

#endif // IR_MODEL_H
