#ifndef IR_MODEL_IEF_H
#define IR_MODEL_IEF_H

#include <QObject>
#include "dblp/irmodel/base/ir_model.h"
#include <dblp/index/models/reference/index_term_ref.h>
#include <commons/log/loggable/loggable.h>

class IRModelIef : public QObject, public IRModel, protected Loggable {
	Q_OBJECT

public:
	~IRModelIef() override;
	IRModelIef(IndexHandler *indexHandler);

	void init(bool lazy = false);

	float termScore(const QString &term) override;
	float bonusFactorPerPhraseTerm() override;
	float bonusFactorForPublicationMatch() override;
	float bonusFactorForVenueMatch() override;
	float bonusFactorForPublicationVenueMatch() override;

	IndexHandler * index();

signals:
	void initStarted();
	void initProgress(double progress);
	void initEnded();

protected:
	LOGGING_OVERRIDE

private:
	void computeIefs();
	float computeIef(const QString &term);
	float computeIef(const QMap<QString, IndexTermRef>::const_iterator &vocabularyEntry);
	void printIefs();

	QHash<QString, float> mIefs;	// := inverse element frequency
									// log10(|E|/ef_t)
};

#endif // IR_MODEL_IEF_H
