CC = gcc

.all: apifs

apifs: src/main.c src/station_list.c src/gios_api.c
	$(CC) src/main.c src/station_list.c src/gios_api.c `pkg-config libcurl json-c fuse3 --cflags --libs` -o src/apifs