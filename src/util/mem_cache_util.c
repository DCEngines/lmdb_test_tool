#include "util/mem_cache_util.h"

extern int memcache_port;


memcached_st* mem_cache_init()
{
  //creating a memcached_st object
	memcached_st *memc = memcached_create(NULL);
	if (memc == NULL){
    fprintf(stderr, "Failed to create memc handle\n");
		return NULL;
	}
  
  //appending the list of memcached servers
	memcached_server_st *servers = NULL;
	servers = memcached_server_list_append(servers, "localhost", memcache_port, &rc);

  //pushing the servers
  rc = memcached_server_push(memc, servers);

  if (rc == MEMCACHED_SUCCESS)
    fprintf(stderr, "Added server successfully\n");
  else
  {
    fprintf(stderr, "Couldn't add server: %s\n", memcached_strerror(memc, rc));
		//TODO: close handle here
		if (memc != NULL) {
			memcached_free(memc);
		}
		return NULL;
  }

	return memc;
}

int mem_cache_free(memcached_st* memc)
{
	if (memc == NULL) {
		return 1;
	}
	else
	{
		memcached_free(memc);
		return 0;
	}
}
     
int mem_cache_put(memcached_st *memc, char* key, char* value)
{
	if (value == NULL)
	{
		printf("Error: value is NULL\n");
		return 1;
	}

	rc = memcached_set(memc, key, strlen(key), value, strlen(value), (time_t)0, (uint32_t)0);

	if (rc == MEMCACHED_SUCCESS)
	{
		//fprintf(stderr, "Key stored successfully\n");
		return 0;
	}	
	else
	{
		fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc, rc));
		return 1;
	}
}

int mem_cache_get(memcached_st *memc, char* key)
{
	retrieved_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);
	if (rc == MEMCACHED_SUCCESS){
		printf("Key: %s found !\n", key);
		printf("Value: %s\n", retrieved_value);
		printf("**********************\n");
		return 0;
	}	
	else {
		printf("Key: %s not found !", key);
		return 1;
	}
}
