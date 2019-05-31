#include "ir_model.h"

IRModel::~IRModel() {}

IRModel::IRModel(IndexHandler &indexHandler) : mIndex(indexHandler) {}

float IRModel::bonusFactorPerPhraseTerm() const
{
	return 1; // no bonus
}

float IRModel::bonusFactorForPublicationMatch() const
{
	return 1; // no bonus
}

float IRModel::bonusFactorForVenueMatch() const
{
	return 1; // no bonus
}

float IRModel::bonusFactorForPublicationVenueMatch() const
{
	return 1; // no bonus
}

float IRModel::bonusFactorForElementFieldFilter() const
{
	return 1; // no bonus
}

float IRModel::bonusFactorForElementFilter() const
{
	return 1; // no bonus
}

IndexHandler &IRModel::index()
{
	return mIndex;
}
