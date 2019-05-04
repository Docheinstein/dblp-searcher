#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextDocument>
#include <Qt>
#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/xml_parser.h"
#include "dblp/index/handler/index_handler.h"
#include "commons/shared/shared.h"
#include "commons/util/util.h"

#include "tests.cpp"
#include <dblp/query/resolver/query_resolver.h>
#include <dblp/query/query/query.h>
#include <dblp/irmodel/ir_model.h>

int doIndexing(
			QString dblpXmlPath,
			QString indexPath,
			QString baseIndexName) {
//	QString dblpXmlPath(argv[2]);
//	QString indexPath(argv[3]);
//	QString baseIndexName =argv[4];

	Indexer indexer(indexPath, baseIndexName);
	XmlParser parser(dblpXmlPath, indexer);
	parser.parse();

	return 0;
}

int doSearch(
			QString indexPath,
			QString baseIndexName) {

	IndexHandler handler(indexPath, baseIndexName);
	IRModel irmodel(&handler);
	QueryResolver resolver(&irmodel);
	Query q("Information retrieval phThesis:\"Harald is bad\" venue.title:dog");

	QSet<QueryMatch> matches = resolver.resolveQuery(q);

	int i = 0;
	foreach (QueryMatch match, matches) {
		qDebug() << "Query match (" << ++i << "): Element " << match.elementId;
	}

//	QString q1 = "Stefano";
//	QString q2 = "Enrico";

//	QSet<ElementFieldMatch> matches1;
//	QSet<ElementFieldMatch> matches2;

//	handler.findElements(q1, ArticleAuthor, matches1);
//	handler.findElements(q2, ArticleAuthor, matches2);

//	foreach (ElementFieldMatch match, matches1) {
//		qDebug() << "Matched element (" << q1 << ") id: " << match.elementId
//				 << " (tf: " << match.termCount << ")";
//	}

//	foreach (ElementFieldMatch match, matches2) {
//		qDebug() << "Matched element (" << q2 << ") id: " << match.elementId
//				 << " (tf: " << match.termCount << ")";
//	}

	return 0;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		qCritical() << "Please provide valid parameters." << endl
					<< "Use with one of the following mode: " << endl
					<< "--index <dblp.xml> <out_index_path>" << endl
					<< "--search <in_index_path>" << endl;
	}
	try {
		QString mode(argv[1]);
		if (mode == "--index")
			return doIndexing(argv[2], argv[3], argv[4]);
		else if (mode == "--search")
			return doSearch(argv[2], argv[3]);
		else
			qCritical() << "Unsupported mode: " << mode;
	} catch (const char * e){
	  qCritical() << "Exception occured; aborting for the following reason:"
				  << endl << ">> " << e;
	  return 0;
	}

	QApplication a(argc, argv);
	MainWindow w;
	w.show();

	return a.exec();
}
