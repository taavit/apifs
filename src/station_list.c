#include "station_list.h"

void
station_list_initialize(struct StationList* list)
{
	list->length = 0;
	list->first = NULL;
	list->last = NULL;
}

void
station_list_cleanup(struct StationList* list)
{
	struct Station *station = list->first;
	struct Station *next = NULL;
	while (station != NULL) {
		next = station->next;
		delete_station(station);
		station = next;
	}
	list->first = NULL;
	list->last = NULL;
}

struct Station*
create_station(uint16_t id, const char *name)
{
	struct Station* station = malloc(sizeof(struct Station));

	station->id = id;
	station->name = strdup(name);
	station->next = NULL;

	return station;
}

void
delete_station(struct Station* station)
{
	free(station->name);
	free(station);
}

void
print_station(struct Station* station)
{
	printf("%d\t%s\n", station->id, station->name);
}

void
print_list(struct StationList* list)
{
	struct Station *station = list->first;
	while (station != NULL) {
		print_station(station);
		station = station->next;
	}
}

void
station_list_push(struct StationList *list, struct Station* element)
{
	if (list->first == NULL) {
		list->first = element;
		list->last = element;
		list->length = 1;
		return;
	}

	list->last->next = element;
	list->last = element;
	list->length++;
}
