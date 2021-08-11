#define FUSE_USE_VERSION 31

#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <fuse3/fuse.h>
#include <fcntl.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include "station_list.h"
#include "gios_api.h"

static struct options {
	int show_help;
} options;

struct StationList stationList;

#define OPTION(t, p)                           \
    { t, offsetof(struct options, p), 1 }
static const struct fuse_opt option_spec[] = {
	OPTION("-h", show_help),
	OPTION("--help", show_help),
	FUSE_OPT_END
};

static void
*api_init(	struct fuse_conn_info *conn,
			struct fuse_config *cfg)
{
	(void) conn;
	cfg->kernel_cache = 1;
	return NULL;
}

static int
api_getattr(	const char *path, struct stat *stbuf,
				struct fuse_file_info *fi)
{
	(void) fi;

	memset(stbuf, 0, sizeof(struct stat));
	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 2;
		return 0;
	}
	struct Station *to_check = stationList.first;

	while (to_check != NULL) {
		char snum[11];
		sprintf(snum, "%d.json", to_check->id);
		if (strcmp(path+1, snum) == 0) {
			stbuf->st_mode = S_IFREG | 0444;
			stbuf->st_nlink = 1;
			stbuf->st_size = 0;
			return 0;
		}
		to_check = to_check->next;
	}

	return -ENOENT;
}

static int api_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi,
			 enum fuse_readdir_flags flags)
{
	(void) offset;
	(void) fi;
	(void) flags;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0, 0);
	filler(buf, "..", NULL, 0, 0);
	struct Station * station = stationList.first;
	while (station != NULL) {
		char snum[11];
		sprintf(snum, "%d.json", station->id);
		filler(buf, snum, NULL, 0, 0);
		station = station->next;
	}

	return 0;
}

static int api_open(const char *path, struct fuse_file_info *fi)
{
	struct Station * station = stationList.first;
	while (station != NULL) {
		char snum[11];
		sprintf(snum, "%d.json", station->id);
		if (strcmp(path+1, snum) == 0) {
			return 0;
		}
		station = station->next;
	}

	if ((fi->flags & O_ACCMODE) != O_RDONLY)
		return -EACCES;

	return -ENOENT;
}

static int api_read(const char *path, char *buf, size_t size, off_t offset,
		      struct fuse_file_info *fi)
{
	size_t len;
	(void) fi;

	struct Station * station = stationList.first;
	while (station != NULL) {
		char snum[11];
		sprintf(snum, "%d.json", station->id);
		if (strcmp(path+1, snum) == 0) {
			char * stationRaw = api_read_station(station->id);
			len = strlen(stationRaw);
			memcpy(buf, stationRaw, len);
			free(stationRaw);
			return len;
		}
		station = station->next;
	}
	return -ENOENT;
}

static const struct fuse_operations api_oper = {
	.init		= api_init,
	.getattr	= api_getattr,
	.readdir	= api_readdir,
	.open		= api_open,
	.read		= api_read,
};

int main(int argc, char *argv[])
{
	int ret;
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

	if (fuse_opt_parse(&args, &options, option_spec, NULL) == -1)
		return 1;
	
	if (options.show_help) {
		assert(fuse_opt_add_arg(&args, "--help") == 0);
		args.argv[0][0] = '\0';
	}

	curl_global_init(CURL_GLOBAL_DEFAULT);

	station_list_initialize(&stationList);
	gios_api_read_stations(&stationList);

	curl_global_cleanup();
	ret = fuse_main(args.argc, args.argv, &api_oper, NULL);

	fuse_opt_free_args(&args);
	station_list_cleanup(&stationList);
	return 0;
}
