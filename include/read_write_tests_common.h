#ifndef __READ_WRITE_TESTS_COMMON_H__
#define __READ_WRITE_TESTS_COMMON_H__

#include <stdio.h>
#include <pthread.h>
#include "util/mem_cache_util.h"

#define MAX_VALUE_SIZE 4*1024
//#define MAX_KEYS ((2 * 1024 * 1024)/(4))
#define MAX_KEYS 1000
#define MAX_THREADS 2
#define MALLOC_VALUE() value = (cache_value_t*) malloc(sizeof(cache_value_t)); 
#define FREE_VALUE() if(value != NULL) { free(value); value = NULL; } 
#define VERSION 1
#define INT_SIZE sizeof(int)
#define MAX_VAL_SIZE 5
 
enum SC_ERRORCODES{
	SC_SUCCESS = 0,
	SC_FAILURE = 1
};

typedef struct cache_value{
	long id;
	int version;
}cache_value_t;

int create_value_from_id(cache_value_t*, long, int);
long write_blocks_to_mem_cache(memcached_st *, long , long , int);
long read_blocks_from_mem_cache(memcached_st *, long , long);
int copy_value_to_block(cache_value_t*, char*);

#endif /*__READ_WRITE_TESTS_COMMON_H__*/
