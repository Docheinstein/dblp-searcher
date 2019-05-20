#ifndef IR_MODEL_H
#define IR_MODEL_H

#include <QString>

class IndexHandler;

class IRModel {
public:
	virtual ~IRModel();
	IRModel(IndexHandler &indexHandler);

	// Score of the term (usually tf.ief)
	virtual float termScore(const QString &term) = 0;

	// Bonus to give for phrasal matches based on the word count
	virtual float bonusFactorPerPhraseTerm() const;

	// Bonus based on the match type

	virtual float bonusFactorForPublicationMatch() const;
	virtual float bonusFactorForVenueMatch() const;
	virtual float bonusFactorForPublicationVenueMatch() const;

	IndexHandler & index();

protected:
	IndexHandler &mIndex;
};

#endif // IR_MODEL_H
