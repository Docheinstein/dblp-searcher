#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>
#include <QtGlobal>

// Global helpers

#define QUIT(msg) qFatal("%s\n%s\n%s", "===== ABORTED =====", msg, "===================");

// Handly cast

#define INT(x) static_cast<int>(x)

#define UINT8(x) static_cast<quint8>(x)
#define UINT16(x) static_cast<quint16>(x)
#define UINT32(x) static_cast<quint32>(x)
#define UINT64(x) static_cast<quint64>(x)

#define INT8(x) static_cast<qint8>(x)
#define INT16(x) static_cast<qint16>(x)
#define INT32(x) static_cast<qint32>(x)
#define INT64(x) static_cast<qint64>(x)

#define FLOAT(x) static_cast<float>(x)
#define DOUBLE(x) static_cast<double>(x)

// Handly number to string conversion

#define BIN(x) QString::number(x, 2)
#define DEC(x) QString::number(x)
#define HEX(x) QString::number(x, 16)
#define FLT(x) QString::number(DOUBLE(x), 'g', 2)
#define DBL(x) QString::number(x, 'g', 2)

// Handly cstring functions

#define streq(a, b) (strcmp(a, b) == 0)

// Min/Max

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Handly enum macros

#define ENUM_STRING(...) \
	enum { \
		__VA_ARGS__ \
	} type;

#define ENUM_STRING_SWITCH_BEGIN switch (type) {
#define ENUM_STRING_SWITCH_END }

#define ENUM_STRING_CASE(case_name, case_str) \
	case case_name: { \
		return #case_str; \
	}

#define ENUM_AUTO_STRING_CASE(case_name) ENUM_STRING_CASE(case_name, #case_name)

// Asserts

#define ASSERT_SELECT(_cond, _where, _8, _7, _6, _5, _4, _3, _2, _1, NAME, ...) NAME
#define ASSERT(...) ASSERT_SELECT(\
		__VA_ARGS__, \
		ASSERT8, ASSERT7, ASSERT6, ASSERT5, ASSERT4, ASSERT3, ASSERT2, ASSERT1 \
	)(__VA_ARGS__)

#define ASSERT1(cond, where, what1) \
	Q_ASSERT_X(cond, where, QString(what1).toStdString().c_str())

#define ASSERT2(cond, where, what1, what2) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) \
	).toStdString().c_str())

#define ASSERT3(cond, where, what1, what2, what3) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + QString(what3) \
	).toStdString().c_str())

#define ASSERT4(cond, where, what1, what2, what3, what4) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + \
		QString(what3)  + QString(what4) \
	).toStdString().c_str())

#define ASSERT5(cond, where, what1, what2, what3, what4, what5) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + \
		QString(what3)  + QString(what4) + \
		QString(what5) \
	).toStdString().c_str())

#define ASSERT6(cond, where, what1, what2, what3, what4, what5, what6) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + \
		QString(what3) + QString(what4) + \
		QString(what5) + QString(what6) \
	).toStdString().c_str())

#define ASSERT7(cond, where, what1, what2, what3, what4, what5, what6, what7) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + \
		QString(what3) + QString(what4) + \
		QString(what5) + QString(what6) +\
		QString(what7) \
	).toStdString().c_str())

#define ASSERT8(cond, where, what1, what2, what3, what4, what5, what6, what7, what8) \
	Q_ASSERT_X(cond, where, QString( \
		QString(what1) + QString(what2) + \
		QString(what3) + QString(what4) + \
		QString(what5) + QString(what6) +\
		QString(what7) + QString(what8) \
	).toStdString().c_str())

#endif // GLOBALS_H
