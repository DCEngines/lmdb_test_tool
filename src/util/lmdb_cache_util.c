#include "util/lmdb_cache_util.h"

int mdb_data_put(char *skey, int keySize, void *sval, size_t svalSize){

	MDB_txn *txn = NULL;
	MDB_dbi dbi;
	MDB_val key, data;
	
	//begin lmdb transcation
	rc = mdb_txn_begin(g_ptrLMDBEnv, NULL, 0, &txn);
	
	//open txn
	rc = mdb_open(txn, NULL, 0, &dbi);

	//construct key, val
	key.mv_size = keySize;
	key.mv_data = skey;
	data.mv_size = svalSize;
	data.mv_data = sval;

	printf("svalSize=%ld\n", svalSize);
	
	rc = mdb_put(txn, dbi, &key, &data, 0);
	
	if (rc) {
		printf("mdb_put: %s\n", mdb_strerror(rc));
	}

	rc = mdb_txn_commit(txn);

	if (rc) {
		printf("mdb_txn_commit: (%d) %s\n", rc, mdb_strerror(rc));
	}
	
	mdb_close(g_ptrLMDBEnv, dbi);
	
	return rc;
}

int mdb_data_get(char* key_s, char* value){

	MDB_txn *txn = NULL;
	MDB_dbi dbi;
	MDB_cursor *cursor;
	MDB_val key, data;
	int flag = 0;

	//begin lmdb transaction
	rc = mdb_txn_begin(g_ptrLMDBEnv, NULL, MDB_RDONLY, &txn);

	//open txn
	rc = mdb_open(txn, NULL, 0, &dbi);

	//open cursor
	rc = mdb_cursor_open(txn, dbi, &cursor);

	//iterate data
	while ((rc = mdb_cursor_get(cursor, &key, &data, MDB_NEXT)) == 0) {

		char *strKey = (char *)key.mv_data;

		if(strcmp(strKey, key_s) == 0){
			value = (char*)data.mv_data;
			flag = 1;
			break;
		}
	}
	
	//cleanup
	mdb_cursor_close(cursor);
	mdb_txn_abort(txn);
	mdb_close(g_ptrLMDBEnv, dbi);
	
	if(flag == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int lmdb_cache_init()
{
	rc = mdb_env_create(&g_ptrLMDBEnv);
	if(rc != 0){
		fprintf(stderr, "failed to create LMDB environment handle\n");
		return rc;	
	}

	//open env handle
	rc = mdb_env_open(g_ptrLMDBEnv, lmdbDirPath, 0, OPENDBMODE);
	if(rc != 0){
		fprintf(stderr, "failed to open lmdb\n");
		return rc;	
	}

	return rc;
}

int lmdb_close(){

	mdb_env_close(g_ptrLMDBEnv);
	return 0;
}
