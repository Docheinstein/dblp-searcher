#include "mainwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextDocument>
#include "dblp/index/indexer/indexer.h"
#include "dblp/parse/parser/dblp_xml_parser.h"
#include <dblp/index/handler/handler.h>
#include "commons/shared/shared.h"
#include <Qt>
#include "commons/util/util.h"

struct ADT {
	QString another;
	struct {
		QList<QString> something;
	} model;
};

void write() {
	QFile file("/home/stefano/Documents/vix");
	file.open(QIODevice::WriteOnly);
	QDataStream writer(&file);

	QString ab = "There is a house in new orleans they call the rising sun."
				 "\nÈ eheh";
	QString cd = "Another fancy string";

	writer << ab.toUtf8();
	writer << cd.toUtf8();

	file.close();
}

void read() {
	QFile file("/home/stefano/Documents/vix");
	file.open(QIODevice::ReadOnly);
	QDataStream reader(&file);

	while (!reader.atEnd()) {
		char *in;
		uint len;
		reader.readBytes(in, len);
		qDebug() << "Read: " << QString(in);
		qDebug() << "L: " << len;
		qDebug() << "FSZ: " << file.size();
		qDebug();
	}



	file.close();
	exit(0);
}
//QString decodeEntities( const QString& src )
//{
//	QString ret(src);
//	QRegExp re("&#([0-9]+);");
//	re.setMinimal(true);

//	int pos = 0;
//	while( (pos = re.indexIn(src, pos)) != -1 )
//	{
//		ret = ret.replace(re.cap(0), QChar(re.cap(1).toInt(0,10)));
//		pos += re.matchedLength();
//	}
//	return ret;
//}
void enc() {
	QString s = "ò";
	QTextDocument doc;
	doc.setHtml("&oacute;");
	QString fromPlainText = Qt::convertFromPlainText("&oacute;");

	qDebug() << "fromDoc: " << doc.toHtml();
	qDebug() << "fromDoc: " << doc.toPlainText();
	qDebug() << "fromDoc: " << doc.toRawText();
	qDebug() << "fromPlainText: " << fromPlainText;
//	qDebug() << "decodeEntities: " << decodeEntities("&oacute;");
	exit(0);
}

void delpunct() {
	QString astring = "  There \\ [noreturn]] is; 2^15 a house % & in - new orleans, Ehy / || there. \" I am the man \" (of the world!) ";
	QString escaped = Util::Indexing::sanitizeTerm(astring);

	qDebug() << "astring: " << astring;
	qDebug() << "escaped: " << escaped;
	qDebug() << "pAppagallo: " << Util::Indexing::sanitizeTerm("pAppagallo,");
	exit(0);
}

int main(int argc, char *argv[])
{
//	enc();
//	write();
//	read();
//	qDebug() << Shared::Index::Config::VOCABULARY_REF_COUNT_THRESHOLD;
//	exit(0);
//	delpunct();

	if (argc < 3) {
		qCritical() << "Please provide valid parameters." << endl
					<< "Use with one of the following mode: " << endl
					<< "--index <dblp.xml> <out_index_path>" << endl
					<< "--search <in_index_path>" << endl;
	}
	try {
		QString mode(argv[1]);
		if (mode == "--index") {
			QString dblpXmlPath(argv[2]);
			QString indexPath(argv[3]);
			QString baseIndexName =argv[4];

			Indexer indexer(indexPath, baseIndexName);
			DblpXmlParser parser(dblpXmlPath, indexer);
			parser.parse();
			return 0;
		}
		else if (mode == "--search") {
			QString indexPath(argv[2]);
			QString baseIndexName = argv[3];

			IndexHandler handler(indexPath, baseIndexName);
			handler.load();

			handler.debug_findArticlesOfAuthor("miniero");
			return 0;
		}
		else {
			qCritical() << "Unsupported mode: " << mode;
		}

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
