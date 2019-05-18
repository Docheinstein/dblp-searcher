#include "args.h"
#include "commons/log/loggable/loggable.h"
#include "commons/globals/globals.h"

STATIC_LOGGING(Args, true)

void printArguments() {
	_vv("==== ARGUMENTS ====");
	_vv("Mode: " << (arguments.mode == DblpSearcherMode::Index ? "INDEX" : "SEARCH"));
	_vv("Index folder path: " << arguments.indexFolderPath);
	_vv("Index base name: " << arguments.baseIndexName);
	_vv("DBLP xml path: " << arguments.dblpXmlFilePath);
	_vv("==== ARGUMENTS END ====");
}

void parseArguments(int argc, char *argv[]) {
	arguments.argc = argc;
	arguments.argv = argv;

	for (int i = 1; i < argc; i++) {
		const char *arg = argv[i];

		auto readNextParam = [argc, argv, &i](QString &str, const QString &failReason) {
			i++;
			if (i >= argc) {
				qCritical() << failReason;
				exit(-1);
			}
			str = argv[i];
		};

		auto readModeParams = [&readNextParam]() {
			readNextParam(arguments.indexFolderPath, "Missing mode parameter: index folder path");
			readNextParam(arguments.baseIndexName, "Missing mode parameter: index base name");
		};

		// Mode
		if (streq(arg, "--index") || streq(arg, "-I")) {
			arguments.mode = DblpSearcherMode::Index;
			readNextParam(arguments.dblpXmlFilePath, "Missing mode parameter: dblp file path");
			readModeParams();
		}
		else if (streq(arg, "--search") || streq(arg, "-S")) {
			arguments.mode = DblpSearcherMode::Search;
			readModeParams();
		}

		// Other options

		// Original XML file (only for search)
		else if (streq(arg, "--xml") || streq(arg, "-X")) {
			readNextParam(arguments.dblpXmlFilePath, "Missing xml parameter");
		}
	}
}
