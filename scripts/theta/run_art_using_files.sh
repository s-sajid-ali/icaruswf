#!/bin/bash

#COBALT -A HEP_on_HPC
#COBALT -n 256
#COBALT -t 06:00:00
#COBALT --mode script

export NUM_DAEMON_NODES=1
export NUM_DAEMON_TOTAL_RANKS=$(($NUM_DAEMON_NODES*16))
export NUM_DAEMON_HYPERTHREADS=1
export NUM_DAEMON_RANKS_PER_NODE=$(($NUM_DAEMON_TOTAL_RANKS/$NUM_DAEMON_NODES))
export NUM_DAEMON_THREADS_PER_RANK=$(($NUM_DAEMON_HYPERTHREADS*64/$NUM_DAEMON_RANKS_PER_NODE))

export ICARUSWF_SRC=/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/ #need to check where is the checked out repository 
export ICARUSWF_BUILD=/projects/HEP_on_HPC/sajid/icarus_hepnos/icaruswf/build

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

# activate spack environment
. /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
spack env activate icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}

# for geometry files!
export
FW_SEARCH_PATH=${FW_SEARCH_PATH}:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}/.spack-env/view/gdml

export NUM_CLIENT_TOTAL_RANKS=376
export NUM_CLIENT_RANKS_PER_NODE=32
export NUM_CLIENT_HYPERTHREADS=1
export NUM_CLIENT_HARDWARE_THREADS_PER_RANK=2

dirname=job_${$COBALT_JOBID}
mkdir dirname
cd dirname
export PATHS_FILE=${ICARUSWF_SRC}/root_file_paths_on_theta.txt
export PROCESS_1_FCL=signalprocessing_root.fcl
export PROCESS_2_FCL=hitfinding_root.fcl
export PROCESS_3_FCL=pandora_root.fcl

printenv > environment.txt

echo "%%% before icaruswf-process-with-files with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"
aprun -n $NUM_CLIENT_TOTAL_RANKS \ # MPI ranks
	-N $NUM_CLIENT_RANKS_PER_NODE \  # MPI ranks/node 
	-d $NUM_CLIENT_HARDWARE_THREADS_PER_RANK \ #
	-j $NUM_CLIENT_HYPERTHREADS \
	-cc none \
	-p ${PDOMAIN} \
	${ICARUSWF_BUILD}/src/modules/mpi_file_processor ${PATHS_FILE} ${PROCESS_1_FCL} ${PROCESS_2_FCL} ${PROCESS_3_FCL} &> process_out
echo "%%% after icaruswf-process-with-files with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"

cd ${BASEDIR}

echo "Destroying protection domain"
apmgr pdomain -r -u ${PDOMAIN}
