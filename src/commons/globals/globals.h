#ifndef GLOBALS_H
#define GLOBALS_H

#include <QString>

// Handly int to string conversion for different bases
#define BIN(x) QString::number(x, 2)
#define DEC(x) QString::number(x)
#define HEX(x) QString::number(x, 16)

// Handly cstring functions

#define streq(a, b) (strcmp(a, b) == 0)

#endif
