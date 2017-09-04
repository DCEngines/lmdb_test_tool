ROOT=.

SRC=${ROOT}/src
OBJ=${ROOT}/obj
BIN=${ROOT}/bin
INC=${ROOT}/include

FLAGS= -g -I
INCLUDES=${INC} 
LIBS= -l memcached -l pthread

all:: clean build

build:
	gcc -c ${FLAGS} ${INCLUDES} ${SRC}/read_write_tests/read_write_util.c -o ${OBJ}/read_write_tests/read_write_util.o
	gcc -c ${FLAGS} ${INCLUDES} ${SRC}/util/mem_cache_util.c -o ${OBJ}/util/mem_cache_util.o -l memcached 
	gcc -c ${FLAGS} ${INCLUDES} ${SRC}/util/lmdb_cache_util.c -o ${OBJ}/util/lmdb_cache_util.o 
	gcc ${FLAGS} ${INCLUDES} ${OBJ}/util/mem_cache_util.o ${OBJ}/read_write_tests/read_write_util.o ${SRC}/read_write_tests/run_test.c -o ${BIN}/rw_test_run ${LIBS} 
	gcc ${FLAGS} ${INCLUDES} ${OBJ}/util/mem_cache_util.o ${OBJ}/read_write_tests/read_write_util.o ${SRC}/read_write_tests/run_test_threads.c -o ${BIN}/rw_test_run_threads ${LIBS}

clean:
	rm -f ${OBJ}/util/*.o
	rm -f ${OBJ}/read_write_tests/*.o
	rm -f ${BIN}/rw_test_run
	rm -f ${BIN}/rw_test_run_threads 
