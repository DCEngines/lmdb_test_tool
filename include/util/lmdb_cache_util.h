#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include "lmdb.h"

#define OPENDBMODE 0777

MDB_env *g_ptrLMDBEnv;
char lmdbDirPath[] = "/lmdb-data";
int rc = 0;

int lmdb_cache_init();
int lmdb_close();
int mdb_data_get(char*, char*);
int mdb_data_put(char*, int, void*, size_t);
