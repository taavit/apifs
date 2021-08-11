#ifndef STATION_LIST_H_
#define STATION_LIST_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Station {
	uint16_t id;
	char* name;
	struct Station* next;
};

struct StationList {
	struct Station* first;
	struct Station* last;
	size_t length;
};

struct Sensor {
	uint16_t id;
	uint16_t stationId;
	char* paramName;
	char* paramFormula;
	char* paramCode;
	uint16_t idParam;
};

struct SensorList {
	struct Station* first;
	struct Station* last;
	size_t length;
};

void station_list_initialize(struct StationList* list);
void station_list_cleanup(struct StationList* list);
struct Station* create_station(uint16_t id, const char *name);
void station_list_push(struct StationList *list, struct Station* element);
void print_list(struct StationList* list);
void print_station(struct Station* station);
void delete_station(struct Station* station);

struct Sensor*
station_sensor_initialize(
	uint16_t id,
	uint16_t stationId,
	const char* paramName,
	const char* paramFormula,
	const char* paramCode,
	uint16_t idParam
);

void station_sensor_cleanup(struct Sensor * sensor);

struct SensorList * sensor_list_initialize();
void sensor_list_cleanup(struct SensorList * list);
void sensor_list_push(struct Sensor * sensor);


#endif // STATION_LIST_H_