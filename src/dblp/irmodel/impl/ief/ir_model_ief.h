#ifndef IR_MODEL_IEF_H
#define IR_MODEL_IEF_H

#include <QObject>
#include "dblp/irmodel/base/ir_model.h"
#include <commons/logger/logger.h>
#include <dblp/index/models/reference/index_term_ref.h>

class IRModelIef : public QObject, public IRModel {
	Q_OBJECT

public:
	~IRModelIef() override;
	IRModelIef(IndexHandler *indexHandler);

	void init(bool lazy = false);

	float termScore(const QString &term) override;
	float bonusFactorPerPhraseTerm() override;

	IndexHandler * index();

signals:
	void initStarted();
	void initProgress(double progress);
	void initEnded();

private:
	static Logger L;

	void computeIefs();
	float computeIef(const QString &term);
	float computeIef(const QString &term, const IndexTermRef &ref);
	void debug_printIefs();

	QHash<QString, float> mIefs;	// := inverse element frequency
									// log10(|E|/ef_t)
};

#endif // IR_MODEL_IEF_H
