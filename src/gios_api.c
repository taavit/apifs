#include "gios_api.h"

struct MemoryChunk {
	char* memory;
	size_t size;
};

static size_t
WriteCb(void *data, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryChunk *chunk = (struct MemoryChunk*) userp;
	char *ptr = realloc(chunk->memory, realsize + chunk->size + 1);
	if (!ptr) {
		printf("ERROR\n");
		return 0;
	}

	chunk->memory = ptr;
	memcpy(&(chunk->memory[chunk->size]), data, realsize);
	chunk->size += realsize;
	chunk->memory[chunk->size] = 0;

	return realsize;
}

void
gios_api_read_stations(struct StationList * stationList)
{
	CURL *curl;
	struct MemoryChunk chunk;
	chunk.memory = malloc(1);
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, GIOS_API_URL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
	curl_easy_perform(curl);

	json_object* root = json_tokener_parse(chunk.memory);
	size_t sstations = json_object_array_length(root);
	printf("Array size: %zu\n", sstations);
	station_list_initialize(stationList);

	for (size_t i=0; i<sstations; i++) {
		json_object* currentStation = json_object_array_get_idx(root, i);
		struct Station* station = create_station(
			json_object_get_int(json_object_object_get(currentStation, "id")),
			json_object_get_string(json_object_object_get(currentStation, "stationName"))
		);

		station_list_push(stationList, station);
	}

	free(chunk.memory);
	json_object_put(root);

	curl_easy_cleanup(curl);
}

void
gios_api_read_station(char * data, uint16_t stationId)
{
	CURL *curl;
	struct MemoryChunk chunk;
	chunk.memory = malloc(1);
	chunk.size = 0;
	char path[255];

	sprintf(path, GIOS_STATION_URL, stationId);

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, path);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCb);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *) &chunk);
	curl_easy_perform(curl);

	data = strdup(chunk.memory);

	free(chunk.memory);

	curl_easy_cleanup(curl);
}
