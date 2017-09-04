#include <libmemcached/memcached.h>
#include <stdio.h>
#include <string.h>

memcached_st *memc;
memcached_return rc;

size_t value_length;
uint32_t flags;

char* retrieved_value;


memcached_st * mem_cache_init();
int mem_cache_put(memcached_st *memc, char*, char*);
int mem_cache_get(memcached_st *memc, char*);
int mem_cache_free(memcached_st *memc);
