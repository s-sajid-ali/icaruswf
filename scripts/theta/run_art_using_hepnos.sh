#!/bin/bash

#COBALT -A HEP_on_HPC
#COBALT -n 4
#COBALT -t 01:00:00
#COBALT --mode script

export NUM_DAEMON_NODES=1
export NUM_DAEMON_TOTAL_RANKS=$(($NUM_DAEMON_NODES*16))
export NUM_DAEMON_HYPERTHREADS=1
export NUM_DAEMON_RANKS_PER_NODE=$(($NUM_DAEMON_TOTAL_RANKS/$NUM_DAEMON_NODES))
export NUM_DAEMON_THREADS_PER_RANK=$(($NUM_DAEMON_HYPERTHREADS*64/$NUM_DAEMON_RANKS_PER_NODE))

PDOMAIN=hepnos-sajid
SSGFILE=hepnos.ssg
export CONNECTION_FILE=${PWD}/connection.json
export ICARUSWF_SRC=/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/ #need to check where is the checked out repository 
export ICARUSWF_BUILD=/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build

rm -f ${SSGFILE}
rm -f ${CONNECTION_FILE}

export MPICH_GNI_NDREG_ENTRIES=1024
export MPICH_MAX_THREAD_SAFETY=multiple
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_SRC}/fcl:${FHICL_FILE_PATH} # needs to be changed to ICARUSWF_SRC once we remove all the in files

echo "%%% start $(date)"
module unload darshan
module swap PrgEnv-intel PrgEnv-gnu
module load gcc/9.3.0 
module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
echo "%%% after_module_loads $(date)"

echo "Setting up protection domain"
apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}

export ICARUS_VERSION=09_37_02_vecmt04
export HEPNOS_VERSION=0_7_2
export MERCURY_TRANSPORT_PROTOCOL="ofi+gni" #tcp for csresearch and gni for Theta

# activate spack environment
. /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
spack env activate icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}

# for geometry files!
export
FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}/.spack-env/view/gdml

# options to collect diagnostic profiles
export MARGO_ENABLE_DIAGNOSTICS=0
export MARGO_ENABLE_PROFILING=0

echo "%%% before_start_daemon $(date)"
aprun -n $NUM_DAEMON_TOTAL_RANKS \
      -N $NUM_DAEMON_RANKS_PER_NODE \
      -d $NUM_DAEMON_THREADS_PER_RANK \
      -j $NUM_DAEMON_HYPERTHREADS \
      -cc none \
      -p ${PDOMAIN} \
      bedrock ${MERCURY_TRANSPORT_PROTOCOL} -c hepnos.json -v info &> server-log &
sleep 30
echo "%%% after_start_daemon $(date)"
while [ ! -f ${SSGFILE} ]; do sleep 10 && echo "waiting for ssgfile"; done

echo "%%% before_start_list_dbs $(date)"
aprun -n 1 \
      -N 1 \
	  -p ${PDOMAIN} \
	  hepnos-list-databases ${MERCURY_TRANSPORT_PROTOCOL} -s ${SSGFILE} > ${CONNECTION_FILE}
sleep 2
echo "%%% after_start_list_dbs $(date)"
while [ ! -f ${CONNECTION_FILE} ]; do sleep 10; done
sed -i '$ d' ${CONNECTION_FILE} # we have to because aprun adds a line

# Create Queues
aprun -n 1 \
      -N 1 \
      -p ${PDOMAIN} \
      ${ICARUSWF_BUILD}/src/modules/cheesyQueue_maker ${MERCURY_TRANSPORT_PROTOCOL} ${CONNECTION_FILE} DetSim
      SignalProcessing HitFinding Pandora &> queue-creation-log

export NUM_CLIENT_NODES=3
export NUM_CLIENT_TOTAL_RANKS=$(($NUM_CLIENT_NODES*32))
export NUM_CLIENT_RANKS_PER_NODE=$(($NUM_CLIENT_TOTAL_RANKS/$NUM_CLIENT_NODES))
export NUM_CLIENT_HYPERTHREADS=1
export NUM_CLIENT_HARDWARE_THREADS_PER_RANK=2

dirname=job_${$COBALT_JOBID}
mkdir dirname
cd dirname
export PROCESS_1_FCL=signalprocessing_hepnos.fcl
export PROCESS_2_FCL=hitfinding_hepnos.fcl
export PROCESS_3_FCL=pandora_hepnos.fcl


cp $CONNECTION_FILE .

export EVENTS_FILE=${ICARUSWF_SRC}/events.txt
export LOADER_FCL_FILE=storedata_queue.fcl #this file is on FHICL_FILE_PATH 
printenv > environment.txt

echo "%%% before icaruswf-load with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"
aprun -n $NUM_CLIENT_TOTAL_RANKS \
      -N $NUM_CLIENT_RANKS_PER_NODE \
  	  -d $NUM_CLIENT_HARDWARE_THREADS_PER_RANK \
      -j $NUM_CLIENT_HYPERTHREADS \
      -cc none \
      -p ${PDOMAIN} \
      ${ICARUSWF_BUILD}/src/modules/mpi_loader ${LOADER_FCL_FILE} ${EVENTS_FILE} &> loader_out 
echo "%%% after icaruswf-load with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"

echo "%%% before icaruswf-process with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"
aprun -n $NUM_CLIENT_TOTAL_RANKS \
	-N $NUM_CLIENT_RANKS_PER_NODE \
	-d $NUM_CLIENT_HARDWARE_THREADS_PER_RANK \
	-j $NUM_CLIENT_HYPERTHREADS \
	-cc none \
	-p ${PDOMAIN} \
	${ICARUSWF_BUILD}/src/modules/mpi_hepnos_processor ${PROCESS_1_FCL} ${PROCESS_2_FCL} ${PROCESS_3_FCL} &> process_out
echo "%%% after icaruswf-process with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"

cd ${BASEDIR}

echo "Destroying protection domain"
apmgr pdomain -r -u ${PDOMAIN}
