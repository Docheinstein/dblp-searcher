#include "loader.h"

#include <dblp/index/handler/index_handler.h>
#include <dblp/irmodel/impl/ief/ir_model_ief.h>
#include <dblp/query/resolver/query_resolver.h>

Loader::Loader(const QString &indexPath, const QString &baseIndexName)
{
	mIndexPath = indexPath;
	mBaseIndexName = baseIndexName;
}

void Loader::run()
{
	IndexHandler handler(mIndexPath, mBaseIndexName);
	IRModelIef irmodel(handler);
	QueryResolver resolver(&irmodel);
	sleep(1);

	emit loadFinished();
}
