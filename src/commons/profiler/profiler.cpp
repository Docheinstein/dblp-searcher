#include "profiler.h"

#include <algorithm>
#include "commons/globals/globals.h"
#include "commons/log/logger/logger.h"

static int profCurrentEntityNumber;
ProfEntity PROF_ENTITIES[MAX_PROF_ENTITIES];

int profRegisterEntity(const char *id, const char *prefix)
{
	if (profCurrentEntityNumber >= MAX_PROF_ENTITIES)
		return profCurrentEntityNumber;

	char *realEntityId;

	if (!prefix) {
		// No prefix
		realEntityId = new char[strlen(id) + 1];
		sprintf(realEntityId, "%s", id);
	}
	else {
		// With prefix
		realEntityId = new char[strlen(prefix) + strlen(id) + 1];
		sprintf(realEntityId, "%s%s", prefix, id);
	}

	int entityNumber = profCurrentEntityNumber;

	PROF_ENTITIES[entityNumber].id = realEntityId;
	PROF_ENTITIES[entityNumber].time = 0;

	profCurrentEntityNumber++;

	return entityNumber;
}

void profPrintEntities(ProfEntity *entities)
{

	// Just for formatting purpose, find the longest function name
	int longestEntityName = 0;
	for (int i = 0; i < profCurrentEntityNumber; ++i) {
		int entityNameLength = INT(strlen(entities[i].id));
		if (entityNameLength > longestEntityName)
			longestEntityName = entityNameLength;

	}

	_ii_("============ PROFILER ============");

	for (int i = 0; i < profCurrentEntityNumber; ++i) {
		const ProfEntity &fun = entities[i];

		char *paddedEntityName = new char[longestEntityName + 1];
		sprintf(paddedEntityName, "%-*s", longestEntityName, fun.id);

		QString timeStr = fun.time > 0 ? (DEC(fun.time / 1000000) + "ms") : "...";
		_ii_(paddedEntityName << "\t|\t" << timeStr);

		delete[] paddedEntityName;
	}

	_ii_("==================================");
}

void profPrint()
{
	profPrintEntities(PROF_ENTITIES);
}

void profPrintSorted()
{

	ProfEntity sortedProfEntities[MAX_PROF_ENTITIES];

	for (int i = 0; i < profCurrentEntityNumber; ++i) {
		sortedProfEntities[i] = PROF_ENTITIES[i];
	}

	std::sort(sortedProfEntities,
			  sortedProfEntities + profCurrentEntityNumber,
			  [](const ProfEntity &pe1, const ProfEntity &pe2) -> bool {
		return pe1.time > pe2.time;
	});

	profPrintEntities(sortedProfEntities);
}

void profReset()
{
	_ii_("PROFILER: reset (" << profCurrentEntityNumber << ")");

	for (int i = 0; i < profCurrentEntityNumber; ++i) {
		PROF_ENTITIES[i].time = 0;
	}
}
