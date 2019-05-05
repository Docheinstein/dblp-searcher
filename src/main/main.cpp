#include <QDebug>
#include <QTextDocument>
#include <Qt>
#include <QCoreApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "dblp/index/indexer/indexer.h"
#include "dblp/xml/parser/xml_parser.h"
#include "dblp/index/handler/index_handler.h"
#include "commons/config/config.h"
#include "commons/util/util.h"

#include "tests.cpp"
#include <dblp/query/resolver/query_resolver.h>
#include <dblp/query/query/query.h>
#include <dblp/irmodel/impl/ief/ir_model_ief.h>
#include <QQmlComponent>
#include "gui/splash/splash_window.h"
#include "gui/main/main_window.h"
#include <QThread>
#include <QtConcurrentRun>
#include "main_gui.h"

int doIndexing(
			QString dblpXmlPath,
			QString indexPath,
			QString baseIndexName) {
	Indexer indexer(indexPath, baseIndexName);
	XmlParser parser(dblpXmlPath, indexer);
	parser.parse();

	return 0;
}

int doSearch(
			QString indexPath,
			QString baseIndexName) {

	IndexHandler handler(indexPath, baseIndexName);
	IRModelIef irmodel(&handler);
	QueryResolver resolver(&irmodel);
//	Query q("Information retrieval phThesis:\"Harald is bad\" venue.title:dog");
	Query q("Stefano Encyclopedia IPTComm");

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

void abort() {
	qCritical() << "Please provide valid parameters." << endl
				<< "Use with one of the following mode: " << endl
				<< "--index <dblp.xml> <out_index_path>" << endl
				<< "--search <in_index_path>" << endl;
	exit(-1);
}

int main(int argc, char *argv[])
{
//	QApplication a(argc, argv);
	if (argc < 2) {
		abort();
	}
	try {
		QString mode(argv[1]);
		if (mode == "--index") {
			if (argc < 5)
				abort();
			return doIndexing(argv[2], argv[3], argv[4]);
		}
		else if (mode == "--search") {
			if (argc < 4)
				abort();
			return doSearch(argv[2], argv[3]);
		}
		else if (mode == "--search-gui")
			return mainGui(argc, argv, argv[2], argv[3]);
		else
			qCritical() << "Unsupported mode: " << mode;
	} catch (const char * e){
	  qCritical() << "Exception occured; aborting for the following reason:"
				  << endl << ">> " << e;
	  return 0;
	}
}
