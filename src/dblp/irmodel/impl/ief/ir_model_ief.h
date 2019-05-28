#ifndef IR_MODEL_IEF_H
#define IR_MODEL_IEF_H

#include <QObject>

#include "commons/log/loggable/loggable.h"
#include "dblp/irmodel/base/ir_model.h"
#include <dblp/index/models/reference/index_term_ref.h>
#include "dblp/index/handler/index_handler.h"

class IRModelIef : public QObject, public IRModel, protected Loggable {
	Q_OBJECT

public:
	~IRModelIef() override;
	IRModelIef(IndexHandler &indexHandler);

	void init();

	float termScore(const QString &term) override;
	float bonusFactorPerPhraseTerm() const override;
	float bonusFactorForPublicationMatch() const override;
	float bonusFactorForVenueMatch() const override;
	float bonusFactorForPublicationVenueMatch() const override;

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
	float computeIef(const VOCABULARY_ADT<QString, IndexTermRef>::const_iterator &vocabularyEntry);

	void printIefs() const;

	QHash<QString, float> mIefs;	// := inverse element frequency
									// log10(|E|/ef_t)
};

#endif // IR_MODEL_IEF_H
