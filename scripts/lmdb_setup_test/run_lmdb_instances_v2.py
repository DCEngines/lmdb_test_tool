import subprocess
import os

MDBPATH="/home/abhishek/Downloads/memcacheddb-lmdb-tests/repo/memcachedb-lmdb/memcachedb"
MDBPORT = 21201
MDBINSPATH = "/lmdb-instances/inst{0}"
MDBPIDFILEPATH="/lmdb-instances/inst{0}/{1}.pid"
MDBCMD = "sudo {0} -p {1} -r -u root" \
                    " -H {2}" \
                    " -m {3} -N -P {4} -t 100 -v -d"

MDBHOST="127.0.0.1"
CONCURRENCY=100
EXECUTIONS=1000000
WINSIZE="10k"

MEMASLAP= "memaslap --servers={0}:{1}"  \
          " --concurrency={2}"  \
          " --execute_number={3}"   \
          " --win_size={4}"


def run_instances(nInstances, dbSizeInMB):
    global MDBPORT, MDBPATH, MDBINSPATH, MDBCMD
    for x in range(nInstances):
        port = MDBPORT + x
        mdbPidFilePath = MDBPIDFILEPATH.format(x, port)
        mdbDirPath = MDBINSPATH.format(x)
        if not os.path.exists(mdbDirPath):
            #remove the existing dir
            #os.rmdir(mdbDirPath)
            os.makedirs(mdbDirPath)

        cmd = MDBCMD.format(MDBPATH, port, mdbDirPath, dbSizeInMB, mdbPidFilePath)
        try:
            print cmd
            #subprocess.call(["touch", mdbPidFilePath], shell=True)
            subprocess.call([cmd], shell=True)
        except Exception as ex:
            print "Exception: ", ex
            pass

    return 0

TESTCMD="nohup /home/mukul/Stat_Util/bin/rw_test_run_threads {0} > ./output/out_{1}.txt 2>&1"

def run_perf_test(nInstances):
    global MDBPORT, MDBPATH, MDBINSPATH, MDBCMD
    cmdlist = list()
    for x in range(nInstances):
        port = MDBPORT + x
        #cmd = MEMASLAP.format(MDBHOST, port, CONCURRENCY, EXECUTIONS, WINSIZE)
        cmd = TESTCMD.format(port, port)
        try:
            #print cmd
            print cmd + " &"
            #cmdlist.insert(x, cmd)
            #subprocess.call([cmd], shell=True)
        except Exception as ex:
            print "Exception: ", ex
            pass

    #try:
        #print cmdlist
        #subprocess.Popen(cmdlist)
        #subprocess.Popen.wait()
    #except Exception as ex:
        #print "Exception: ", ex
    return 0

run_instances(1, (2 * 1024))
run_perf_test(1)


