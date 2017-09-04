#include "read_write_tests_common.h"

int memcache_port = 21201;

int create_value_from_id(cache_value_t* value, long id, int version)
{
	if (value == NULL){
		return 1;
 	}

	value->id = id;
	value->version = version;
	return 0;
}

int copy_value_to_block(cache_value_t* value, char* block_value)
{
	if (value == NULL || block_value == NULL){
		return 1;
	}
	int no_of_blocks = MAX_VALUE_SIZE / sizeof(value);
	int i = 0;
	char temp_val[MAX_VAL_SIZE];
	char *curr = block_value;

	for (; i < no_of_blocks; i++)
	{
		sprintf(temp_val,"%ld", value->id);
		memcpy(curr, temp_val, strlen(temp_val));

		curr += strlen(temp_val);
		
		sprintf(temp_val, "%d", value->version);
		memcpy(curr, temp_val, strlen(temp_val));
		
		curr += strlen(temp_val);
	}

	return 0;
}

long write_blocks_to_mem_cache(memcached_st *memc, long offset, long maxCount, int version)
{
	int rc = SC_SUCCESS;
	long i = 0;
	cache_value_t* value = NULL;
	char key[MAX_VAL_SIZE];	
	char block_value[MAX_VALUE_SIZE] = {0};
   	
	printf("Writing Blocks to mem_cache...\n");	
	MALLOC_VALUE();

	long endIndex = maxCount + offset;

	for(i = offset; i < endIndex; i++)
	{

		rc = create_value_from_id(value, i, version);	
		if (rc != SC_SUCCESS)
		{
			goto clean;
		}

		rc = copy_value_to_block(value, block_value);
		
		if (rc != SC_SUCCESS)
		{
			goto clean;
		}
	
		sprintf(key, "%ld", i);
		//printf("Adding Key: %s to memcache\n", key);
		rc = mem_cache_put(memc, key, block_value);	
		if (rc != SC_SUCCESS)
		{
			goto clean;
		}
	}

clean:
	FREE_VALUE()	
	return i;
}

long read_blocks_from_mem_cache(memcached_st *memc, long offset, long maxCount)
{
	int rc = SC_SUCCESS; 
	long i = 0;
	char key[MAX_VAL_SIZE];

	printf("Reading Blocks from mem cache...\n");
    long endIndex = offset + maxCount;	
	for (i = offset; i < endIndex; i++){ 
        sprintf(key, "%ld", i); 
	    rc = mem_cache_get(memc, key);
        if (rc != SC_SUCCESS){
 			goto finish;
 		}
	}

finish:
	return i;
}
         
