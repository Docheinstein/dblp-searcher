#include "main_index.h"

#include "main/args/args.h"
#include "dblp/xml/models/element/dblp_xml_element.h"
#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/dblp_xml_parser.h"

int startIndexMode()
{
	Q_ASSERT(arguments.mode == DblpSearcherMode::Index);

	Indexer indexer(arguments.indexFolderPath, arguments.baseIndexName);
	DblpXmlParser parser(arguments.dblpXmlFilePath, indexer);
	parser.parse();

	return 0;
}
