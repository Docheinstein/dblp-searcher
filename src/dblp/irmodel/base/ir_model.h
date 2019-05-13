#ifndef IR_MODEL_H
#define IR_MODEL_H

#include <QString>

class IndexHandler;

class IRModel {
public:
	virtual ~IRModel();
	IRModel(IndexHandler &indexHandler);

	virtual float termScore(const QString &term) = 0;
	virtual float bonusFactorPerPhraseTerm() const;
	virtual float bonusFactorForPublicationMatch() const;
	virtual float bonusFactorForVenueMatch() const;
	virtual float bonusFactorForPublicationVenueMatch() const;

	IndexHandler & index();

protected:
	IndexHandler &mIndex;
};

#endif // IR_MODEL_H
