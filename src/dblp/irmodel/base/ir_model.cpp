#include "ir_model.h"
#include "dblp/index/handler/index_handler.h"

IRModel::~IRModel() {}

IRModel::IRModel(IndexHandler &indexHandler) : mIndex(indexHandler)
{
}

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

IndexHandler &IRModel::index()
{
	return mIndex;
}
