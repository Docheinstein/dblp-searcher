#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

// Active log levels

// TRACE_GLOBAL_LEVEL: add func, filename and line to each message

#define TRACE_GLOBAL_LEVEL		1
#define TRACE_LEVEL				1
#define DEBUG_LEVEL				1
#define VERBOSE_LEVEL			1
#define INFO_LEVEL				1
#define WARN_LEVEL				1
#define ERROR_LEVEL 			1


// SILENT_ALL: shut up every log level
// SILENT_ALMOST_ALL_ALL: shut up every log level apart from info, warn and errors

#define SILENT_ALL			0
#define SILENT_ALMOST_ALL	0


// Real log levels state accordingly to SILENT_ALL and SILENT_ALMOST_ALL

#define TRACE_GLOBAL	(TRACE_GLOBAL_LEVEL && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define TRACE			(TRACE_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define DEBUG			(DEBUG_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define VERBOSE			(VERBOSE_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define INFO			(INFO_LEVEL 	&& !SILENT_ALL)
#define WARN			(WARN_LEVEL	 	&& !SILENT_ALL)
#define ERROR			(ERROR_LEVEL 	&& !SILENT_ALL)

#endif // LOG_CONFIG_H
