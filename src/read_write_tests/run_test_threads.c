#include "read_write_tests_common.h"
#include <time.h>

//clockid_t clk_id = CLOCK_PROCESS_CPUTIME_ID;
clockid_t clk_id = CLOCK_MONOTONIC;

extern int memcache_port;

typedef struct threadInfo {
	int thread_id;
	long st_offset;
	long max_count;
	long totalKeysProcessed;
	memcached_st *memc;
} threadInfo_t;

struct timespec diff(struct timespec start, struct timespec end)
{
				struct timespec temp;
				if ((end.tv_nsec-start.tv_nsec)<0) {
								temp.tv_sec = end.tv_sec-start.tv_sec-1;
								temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
				} else {
								temp.tv_sec = end.tv_sec-start.tv_sec;
								temp.tv_nsec = end.tv_nsec-start.tv_nsec;
				}
				return temp;
}


void *writerProc(void *data) {
	threadInfo_t *pThInfo = (threadInfo_t *)data;
	struct timespec time1, time2, dtime;
	fprintf(stderr, "Thread %d: max_count: %ld\n",
			pThInfo->thread_id,
			pThInfo->max_count);
	
	clock_gettime(clk_id, &time1);
	long lastOffset = write_blocks_to_mem_cache(pThInfo->memc, pThInfo->st_offset, pThInfo->max_count, pThInfo->thread_id);	
	clock_gettime(clk_id, &time2);
	
	if (lastOffset < 0) {
		goto clean;
	}
	
	//calculate total processed by this thread
	pThInfo->totalKeysProcessed = lastOffset - pThInfo->st_offset;

clean:
	dtime = diff(time1,time2);
	fprintf(stderr, "ThreadId=%d, Start=%ld, MaxCount=%ld, TotalKeysProcessed=%ld, time ns=%ld\n", 
							pThInfo->thread_id, 
							pThInfo->st_offset, 
							pThInfo->max_count, 
							pThInfo->totalKeysProcessed,
							dtime.tv_nsec);
	return NULL;
}

void *readerProc(void *data) {
	threadInfo_t *pThInfo = (threadInfo_t *)data;
	struct timespec time1, time2, dtime;
	fprintf(stderr, "Thread %d: offset: %ld max_count: %ld\n",
			pThInfo->thread_id,
			pThInfo->st_offset,
			pThInfo->max_count);

	clock_gettime(clk_id, &time1);
	long lastOffset = read_blocks_from_mem_cache(pThInfo->memc, pThInfo->st_offset, pThInfo->max_count);	
	clock_gettime(clk_id, &time2);

	if (lastOffset < 0) {
		goto clean;
	}
	
	//calculate total keys processed by this thread
	pThInfo->totalKeysProcessed = lastOffset - pThInfo->st_offset;

clean:
	dtime = diff(time1, time2);
	fprintf(stderr, "ThreadId=%d, Start=%ld, MaxCount=%ld, TotalKeysProcessed=%ld, time ns=%ld\n",
							pThInfo->thread_id,
							pThInfo->st_offset,
							pThInfo->max_count,
							pThInfo->totalKeysProcessed,
							dtime.tv_nsec);
	return NULL;
}

int readerWriterThreads(int maxThreads, int readerWriterCode) {

	int rc = 0;
    pthread_t thread[maxThreads + 1]; 
    threadInfo_t threadInfo[maxThreads + 1]; 
    long perthreadsize = MAX_KEYS/maxThreads;
    long offset = 0;
    long totalKeys = MAX_KEYS;
    int i = 0, threads = 0;
    struct timespec time1, time2, dtime;    
    clock_gettime(clk_id, &time1);
    for(i = 0; i < maxThreads; i++) {
        fprintf(stderr, "totalKeys: %zu\n", totalKeys);
        threadInfo[i].thread_id = i;
        threadInfo[i].max_count = perthreadsize;
        threadInfo[i].st_offset = offset;
        offset += threadInfo[i].max_count;
        memcached_st *memc = mem_cache_init();
        if (memc == NULL){
            rc = -1; 
            goto clean;
        }
        threadInfo[i].memc = memc;
        fprintf(stderr, "worker %d : max_count = %ld\n", threadInfo[i].thread_id, threadInfo[i].max_count);
		
		if (readerWriterCode == 0) {
        	rc = pthread_create(&thread[i], NULL, readerProc, &threadInfo[i]);
		}
		else {
			rc = pthread_create(&thread[i], NULL, writerProc, &threadInfo[i]);
		}

        if(rc != SC_SUCCESS){
            fprintf(stderr, "error creating thread %d\n", i); 
            return -1; 
        }
        totalKeys -= threadInfo[i].max_count;
        threads++;
    }   

    if(totalKeys) {
        threadInfo[i].thread_id = i;
        threadInfo[i].max_count = perthreadsize;
        threadInfo[i].st_offset = offset;
        offset += threadInfo[i].max_count;
        memcached_st *memc = mem_cache_init();
        if (memc == NULL){
            rc = -1; 
            goto clean;
        }
        threadInfo[i].memc = memc;
        fprintf(stderr, "worker %d : max_count = %ld\n", threadInfo[i].thread_id, threadInfo[i].max_count);
		
		if (readerWriterCode == 0) {
			rc = pthread_create(&thread[i], NULL, readerProc, &threadInfo[i]);
        }
		else {
			rc = pthread_create(&thread[i], NULL, writerProc, &threadInfo[i]);
		}

		if(rc != SC_SUCCESS){
            fprintf(stderr, "error creating thread %d\n", i); 
            return -1; 
        }
		totalKeys -= threadInfo[i].max_count;
        threads++;
    }

    for(i = 0; i < threads; i++) {
        rc = pthread_join(thread[i], NULL);
        if(rc != SC_SUCCESS){
            fprintf(stderr, "error joining thread %d\n", i);
            return -1;
        }
    }
    clock_gettime(clk_id, &time2);
    dtime = diff(time1,time2);
    fprintf(stderr, "Total Time ns=%ld\n", dtime.tv_nsec);

clean:
    fprintf(stderr, "execution finished status %d\n", rc);
    for(i = 0; i < threads; i++) {
        rc = mem_cache_free(threadInfo[i].memc);
        if (rc != SC_SUCCESS){
            fprintf(stderr, "error freeing memc %d\n", i);
        }
    }
    return 0;
	
}

int main(int argc, char** argv){
	int rc = SC_SUCCESS;

	int maxThreads = MAX_THREADS;
	if (argc > 2){
		memcache_port = atoi(argv[1]);		
		maxThreads = atoi(argv[2]);		
	}

	readerWriterThreads(maxThreads, 0);

clean:
	return 0;
}


#if 0
int main(int argc, char** argv)
{
	int rc = SC_SUCCESS;

	if (argc > 1){
		memcache_port = atoi(argv[1]);		
	}

	memc = mem_cache_init();

	if (memc == NULL)
	{
		goto clean;
	}
	
	rc = write_blocks_to_mem_cache(memc, MAX_KEYS, VERSION);	
	
	if (rc != SC_SUCCESS)
	{
		goto clean;
	}

	rc = read_blocks_from_mem_cache(memc);

	if (rc != SC_SUCCESS)
	{
		goto clean;
	}

clean:
	return 0;
}
#endif
