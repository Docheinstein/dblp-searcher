#ifndef APP_CONFIG_H
#define APP_CONFIG_H

// =============================================================================
// Contains all the config that changes the behaviour of the app at compile time
// =============================================================================

#define VERSION 0.9

// Whether precompute all the ief (slow init, but faster searches) or not
// If set to 1, the ief are not precomputed
#define LAZY_IEF 1

// =============================================================================


// =)
#define EASTER_EGGS 1


// =============================================================================


// Set to 0 or 1 to enable/disable profiler
#define PROFILER 0


// =============================================================================


// Active log levels

// TRACE_GLOBAL_LEVEL: add func, filename and line to each message

#define TRACE_GLOBAL_LEVEL		0
#define TRACE_LEVEL				1
#define DEBUG_LEVEL				1
#define VERBOSE_LEVEL			1
#define INFO_LEVEL				1
#define WARN_LEVEL				1
#define ERROR_LEVEL 			1


// SILENT_ALL: shut up every log level
// SILENT_ALMOST_ALL_ALL: shut up every log level apart from info, warn and errors

#define SILENT_ALL			0
#define SILENT_ALMOST_ALL	1


// Real log levels state accordingly to SILENT_ALL and SILENT_ALMOST_ALL

#define TRACE_GLOBAL	(TRACE_GLOBAL_LEVEL && !SILENT_ALL && !SILENT_ALMOST_ALL)
#define TRACE			(TRACE_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define DEBUG			(DEBUG_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define VERBOSE			(VERBOSE_LEVEL 	&& !SILENT_ALL && !SILENT_ALMOST_ALL)
#define INFO			(INFO_LEVEL 	&& !SILENT_ALL)
#define WARN			(WARN_LEVEL	 	&& !SILENT_ALL)
#define ERROR			(ERROR_LEVEL 	&& !SILENT_ALL)


// =============================================================================

#endif // APP_CONFIG_H
