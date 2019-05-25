#ifndef ARGS_H
#define ARGS_H

#include <QString>

enum class DblpSearcherMode {
	Index,
	Search
};

// Global arguments, accessible from everywhere

extern struct Arguments {
	DblpSearcherMode mode;
	QString dblpXmlFilePath;
	QString indexFolderPath;
	QString baseIndexName;

	int argc;
	char **argv;
} arguments;

void parseArguments(int argc, char *argv[]);
void printArguments();


#endif // ARGS_H
