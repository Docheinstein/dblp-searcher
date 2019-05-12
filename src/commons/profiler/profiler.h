#ifndef PROFILER_H
#define PROFILER_H

#include <QElapsedTimer>

// Set to 0 or 1 to enable/disable profiler
#define PROFILER 1

#define MAX_PROF_ENTITIES 200

// Core macros

#if PROFILER

// PROF_BEGIN

#define PROF_BEGIN_REAL(var, id, prefix) \
	static int PROF_ENTITY_##var = prof_register_entity(id, prefix); \
	QElapsedTimer profTimer_##var; \
	profTimer_##var.start();

// PROF_END

#define PROF_END(id) \
	PROF_ENTITIES[PROF_ENTITY_##id].time += \
		static_cast<quint64>(profTimer_##id.nsecsElapsed());

#else
#define PROF_BEGIN_REAL(_1, _2)
#define PROF_END(_1)
#endif

// Other handly macros

#define PROF_BEGIN(id)		PROF_BEGIN_REAL(id, #id, "[ 0] ")
#define PROF_BEGIN0(id)		PROF_BEGIN_REAL(id, #id, "[ 0] ")
#define PROF_BEGIN1(id)		PROF_BEGIN_REAL(id, #id, "[ 1] | ")
#define PROF_BEGIN2(id)		PROF_BEGIN_REAL(id, #id, "[ 2] | | ")
#define PROF_BEGIN3(id)		PROF_BEGIN_REAL(id, #id, "[ 3] | | | ")
#define PROF_BEGIN4(id)		PROF_BEGIN_REAL(id, #id, "[ 4] | | | | ")
#define PROF_BEGIN5(id)		PROF_BEGIN_REAL(id, #id, "[ 5] | | | | | ")
#define PROF_BEGIN6(id)		PROF_BEGIN_REAL(id, #id, "[ 6] | | | | | | ")
#define PROF_BEGIN7(id)		PROF_BEGIN_REAL(id, #id, "[ 7] | | | | | | | ")
#define PROF_BEGIN8(id)		PROF_BEGIN_REAL(id, #id, "[ 8] | | | | | | | | ")
#define PROF_BEGIN9(id)		PROF_BEGIN_REAL(id, #id, "[ 9] | | | | | | | | | ")
#define PROF_BEGIN10(id)	PROF_BEGIN_REAL(id, #id, "[10] | | | | | | | | | | ")

#define PROF_FUNC_BEGIN		PROF_BEGIN_REAL(__func__, __func__, "[ 0] ")
#define PROF_FUNC_BEGIN0	PROF_BEGIN_REAL(__func__, __func__, "[ 0] ")
#define PROF_FUNC_BEGIN1	PROF_BEGIN_REAL(__func__, __func__, "[ 1] | ")
#define PROF_FUNC_BEGIN2	PROF_BEGIN_REAL(__func__, __func__, "[ 2] | | ")
#define PROF_FUNC_BEGIN3	PROF_BEGIN_REAL(__func__, __func__, "[ 3] | | | ")
#define PROF_FUNC_BEGIN4	PROF_BEGIN_REAL(__func__, __func__, "[ 4] | | | | ")
#define PROF_FUNC_BEGIN5	PROF_BEGIN_REAL(__func__, __func__, "[ 5] | | | | | ")
#define PROF_FUNC_BEGIN6	PROF_BEGIN_REAL(__func__, __func__, "[ 6] | | | | | | ")
#define PROF_FUNC_BEGIN7	PROF_BEGIN_REAL(__func__, __func__, "[ 7] | | | | | | | ")
#define PROF_FUNC_BEGIN8	PROF_BEGIN_REAL(__func__, __func__, "[ 8] | | | | | | | | ")
#define PROF_FUNC_BEGIN9	PROF_BEGIN_REAL(__func__, __func__, "[ 9] | | | | | | | | | ")
#define PROF_FUNC_BEGIN10	PROF_BEGIN_REAL(__func__, __func__, "[10] | | | | | | | | | | ")

#define PROF_FUNC_END PROF_END(__func__);


typedef struct prof_entity {
	const char *id;
	quint64 time;
} prof_entity;

extern int prof_current_entity_number;
extern prof_entity PROF_ENTITIES[MAX_PROF_ENTITIES];

extern int prof_register_entity(const char *func, const char *prefix = nullptr);
extern void prof_print_entities(prof_entity *entities);
extern void prof_print_sorted();
extern void prof_print();
extern void prof_reset();

#endif // PROFILER_H
