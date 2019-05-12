#include "profiler.h"
#include <algorithm>
#include <QDebug>
#include "commons/globals/globals.h"
#include <iterator>

int prof_current_entity_number = 0;
prof_entity PROF_ENTITIES[MAX_PROF_ENTITIES];

int prof_register_entity(const char *id, const char *prefix)
{
	if (prof_current_entity_number >= MAX_PROF_ENTITIES)
		return prof_current_entity_number;

	char *real_entity_id;

	if (!prefix) {
		// No prefix
		real_entity_id = new char[strlen(id) + 1];
		sprintf(real_entity_id, "%s", id);
	}
	else {
		real_entity_id = new char[strlen(prefix) + strlen(id) + 1];
		sprintf(real_entity_id, "%s%s", prefix, id);
	}

	int entity_number = prof_current_entity_number;
	PROF_ENTITIES[entity_number].id = real_entity_id;
	PROF_ENTITIES[entity_number].time = 0;
	prof_current_entity_number++;
	return entity_number;
}


void prof_print_entities(prof_entity *entities)
{

	// Just for formatting purpose, find the longest function name
	int longest_entity_name = 0;
	for (int i = 0; i < prof_current_entity_number; i++) {
		int entity_name_length = INT(strlen(entities[i].id));
		if (entity_name_length > longest_entity_name)
			longest_entity_name = entity_name_length;

	}
	qInfo() << "============ PROFILER ============";

	for (int i = 0; i < prof_current_entity_number; i++) {
		const prof_entity &fun = entities[i];

		char *padded_entity_name = new char[longest_entity_name + 1];
		sprintf(padded_entity_name, "%-*s", longest_entity_name, fun.id);

		QString timeStr = fun.time > 0 ? (DEC(fun.time / 1000000) + "ms") : "...";
		qInfo().noquote() << padded_entity_name << "\t|\t" << timeStr;

		delete[] padded_entity_name;
	}

	qInfo() << "==================================";
}

void prof_print()
{
	prof_print_entities(PROF_ENTITIES);
}

void prof_print_sorted()
{

	prof_entity sorted_prof_entities[MAX_PROF_ENTITIES];

	for (int i = 0; i < prof_current_entity_number; i++) {
		sorted_prof_entities[i] = PROF_ENTITIES[i];
	}

	std::sort(sorted_prof_entities,
			  sorted_prof_entities + prof_current_entity_number,
			  [](const prof_entity &pe1, const prof_entity &pe2) -> bool {
		return pe1.time > pe2.time;
	});

	prof_print_entities(sorted_prof_entities);

}

void prof_reset()
{
	qInfo().nospace() << "PROFILER: reset (" << prof_current_entity_number << ")";

	for (int i = 0; i < prof_current_entity_number; i++) {
		PROF_ENTITIES[i].time = 0;
	}
}
