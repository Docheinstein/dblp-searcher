#include <QList>
#include <QString>
#include <QDebug>
#include <QTextDocument>
#include "dblp/xml/models/types/xml_types.h"
#include "commons/util/util.h"

struct ADT {
	QString another;
	struct {
		QList<QString> something;
	} model;
};

void printAdt(const ADT &adt) {
	qDebug() << "another: " << adt.another;
	foreach (QString smth, adt.model.something) {
		qDebug() << "- something: " << smth;
	}
}

QHash<QString, ADT> hh;
QHash<QString, quint32> h;

void put() {
//	h.insertMulti("pippo", 3);
//	h.insertMulti("pippo", 4);
//	h.insertMulti("pippo", 4);
//	h.insertMulti("pippo", 2);
//	h.insertMulti("pippo", 6);
//	h.insertMulti("pippo", 41);
//	h.insertMulti("pippo", 7);
	ADT a1 {"pippo_another", {{"pippo_something_1", "pippo_something_2"}}};
	ADT a2 {"pluto_another", {}};
	hh.insert("pippo", a1);
	hh.insert("pluto", a2);

	auto it = hh.find("pluto");

	if (it != hh.end()) {
		it->model.something.append("pluto_something_1");
	}
}

void retr() {
//	QList<quint32> pippos = h.values("pippo");
//	for (auto it = pippos.begin(); it != pippos.end(); it++) {
//		qDebug() << "Scanning value: " << *it;

//	}
//	auto it = h.find("pippo");
//	while (it != h.end() && it.key() == "pippo") {
//		qDebug() << "Scanning value: " << it.value();
//		it++;
//	}

	for (auto it = hh.begin(); it != hh.end(); it++) {
		printAdt(*it);
	}

	exit(-1);
}

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
	QString escaped = Util::String::sanitizeTerm(astring);

	qDebug() << "astring: " << astring;
	qDebug() << "escaped: " << escaped;
	qDebug() << "pAppagallo: " << Util::String::sanitizeTerm("pAppagallo,");
	exit(0);
}

void tim() {
	qDebug() << "A time: " << Util::Time::humanTime(239100);
	exit(0);
}

void plread() {
//	QFile plfile;
//	QDataStream plstream;
//	plfile.setFileName("/home/stefano/Develop/Qt/DblpSearcher/res/index/prod.plix");
//	plstream.setDevice(plfile);
//	plfile.open(QFile::ReadOnly);

}
enum Value
{
  Apple,
  Pear,
  Banana,
  Strawberry
};

class Fruit
{
public:


  bool IsYellow() const { return value == Banana; }

  Fruit(Value aFruit) : value(aFruit) { }

  bool operator==(Fruit a) { return value == a.value; }
  bool operator!=(Fruit a) { return value != a.value; }

private:
  Value value;
};

void type() {
	qDebug() << "Art: " << XmlElementType::ARTICLE->string();
	qDebug() << "Bk: " << XmlElementType::BOOK->string();
	qDebug() << "Bk: " << *XmlElementType::BOOK;
	exit(-1);
}

void freeit() {
	int * intptr = new int();
	int * anotherptr = intptr;

	*intptr = 5;
	anotherptr = intptr;

	qDebug() << "v1: " << *intptr;
	qDebug() << "v2: " << *anotherptr;

	delete intptr;
//	delete anotherptr;

	qDebug() << "v1: " << *intptr;
	qDebug() << "v2: " << *anotherptr;

	exit(-1);
}

QString q1 =
R"#(VLDB publication: "lorenzo miniero " "publication.year: 2019 inproc: "computer science" venue: Conf venue.title: "VLDP Hello" there arte)#";
QString q2 = R"#(There is a cat "very fast" above the " great table")#";
QString q3 = R"#(There is a cat "very fast"above the " great table)#";

class Something {
public:
	Something(int &smth) {
		mSomething = smth;
	}

	void whoAreYou() {
		qDebug() << "I am: " << mSomething << "(" << &mSomething << ")";
	}

private:
	int mSomething;
};

void assign(Something *&smth, Something *&smth2, int &a) {
	int b = 5;

	smth = new Something(a);
	smth->whoAreYou();

	smth2 = new Something(b);
	smth2->whoAreYou();

}

void localByRef() {
	Something * smth;
	Something * smth2;

	if (1 > 0) {
		int a = 6;
		assign(smth, smth2, a);
		smth->whoAreYou();
		smth2->whoAreYou();
	}

	smth->whoAreYou();
	smth2->whoAreYou();

	exit(-1);

}
