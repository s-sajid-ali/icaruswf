#!/bin/bash

#COBALT -A HEP_on_HPC
#COBALT -n 256
#COBALT -t 6:00:00
#COBALT --mode script

#please update these to point to the correct directories that you have permissions to
export ICARUSWF_SRC=/projects/HEP_on_HPC/sehrish/icaruswf/ #need to check where is the checked out repository 
export ICARUSWF_BUILD=/projects/HEP_on_HPC/sehrish/icaruswf_build

# -----------------------------------------------------------------------------
# NOTHING BELOW THIS SHOULD NEED MODIFICATION.
# -----------------------------------------------------------------------------
#
export MPICH_GNI_NDREG_ENTRIES=1024
export MPICH_MAX_THREAD_SAFETY=multiple
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_SRC}/fcl:${FHICL_FILE_PATH} # needs to be changed to ICARUSWF_SRC once we remove all the in files

echo "%%% start $(date)"
echo "script is running ${HOSTNAME}"
module unload darshan
module swap PrgEnv-intel PrgEnv-gnu
module load gcc/9.3.0 
module swap cray-mpich/7.7.14 cray-mpich-abi/7.7.14
echo "%%% after_module_loads $(date)"

export ICARUS_VERSION=09_37_02_vecmt04
export HEPNOS_VERSION=0_7_2

# activate spack environment
. /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh
spack env activate icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}

# for geometry files!
# On theta we needed to add the following fw dir path for XML files needed for the pandora step. 
export
FW_SEARCH_PATH=${FW_SEARCH_PATH}:/projects/HEP_on_HPC/icaruscode/spack/opt/spack/cray-cnl7-haswell/gcc-9.3.0/icaruscode-09.37.02.vecmt04-7jkg3tmlm2qywlujplpxvs32wax5uotr/fw:/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/var/spack/environments/icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}/.spack-env/view/gdml

export NUM_CLIENT_TOTAL_RANKS=376
export NUM_CLIENT_RANKS_PER_NODE=32
export NUM_CLIENT_HYPERTHREADS=1
export NUM_CLIENT_HARDWARE_THREADS_PER_RANK=2

export BASEDIR=job_${COBALT_JOBID}
mkdir ${BASEDIR}
pushd ${BASEDIR}

export PATHS_FILE=${ICARUSWF_SRC}/root_file_paths_on_theta.txt
export PROCESS_1_FCL=signalprocessing_root.fcl
export PROCESS_2_FCL=hitfinding_root.fcl
export PROCESS_3_FCL=pandora_root.fcl

printenv > environment.txt

echo "%%% before icaruswf-process-with-files with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"
aprun -n $NUM_CLIENT_TOTAL_RANKS \
	-N $NUM_CLIENT_RANKS_PER_NODE \
	-d $NUM_CLIENT_HARDWARE_THREADS_PER_RANK \
	-j $NUM_CLIENT_HYPERTHREADS \
	-cc none \
	${ICARUSWF_BUILD}/src/modules/mpi_files_processor ${PATHS_FILE} ${PROCESS_1_FCL} ${PROCESS_2_FCL} ${PROCESS_3_FCL} &> process_out
echo "%%% after icaruswf-process-with-files with $NUM_CLIENT_HARDWARE_THREADS_PER_RANK threads, at $(date)"

rm */*.root
popd
tar zcvf ${BASEDIR}.tar.gz ${BASEDIR}/ 
