#ifndef GIOS_API_H_
#define GIOS_API_H_

#include <curl/curl.h>
#include <json-c/json.h>
#include <string.h>

#include "station_list.h"

#define GIOS_API_URL "http://api.gios.gov.pl/pjp-api/rest/station/findAll"
#define GIOS_STATION_URL "http://api.gios.gov.pl/pjp-api/rest/station/sensors/%d"

void gios_api_read_stations(struct StationList * stationList);
void gios_api_read_station(char * data, uint16_t stationId);

#endif // GIOS_API_H_