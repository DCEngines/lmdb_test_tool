#include "read_write_tests_common.h"

extern int memcache_port;

int main(int argc, char** argv)
{
	int rc = SC_SUCCESS;

	if (argc > 1){
		memcache_port = atoi(argv[1]);		
	}

	memcached_st* memc = mem_cache_init();
	if (memc == NULL){
		goto clean;
	}
	
	rc = write_blocks_to_mem_cache(memc, MAX_KEYS, 0, VERSION);	
	
	if (rc != SC_SUCCESS)
	{
		goto clean;
	}

	rc = read_blocks_from_mem_cache(memc, MAX_KEYS, 0);

	if (rc != SC_SUCCESS)
	{
		goto clean;
	}

clean:
	return 0;
}
