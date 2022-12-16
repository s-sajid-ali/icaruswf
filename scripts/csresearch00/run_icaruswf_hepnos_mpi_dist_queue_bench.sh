#!/bin/bash
set -euo pipefail

export MPICH_MAX_THREAD_SAFETY=multiple

# activate spack environment
. /scratch/gartung/spack/share/spack/setup-env.sh
spack load gcc@9.3.0
spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_0
export ICARUSWF_SRC=/nashome/s/sasyed/packages/icaruswf
. ${ICARUSWF_SRC}/envvariable.sh

export ICARUSWF_BUILD=/nashome/s/sasyed/packages/icaruswf/build
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}

export DATA_DIR=/scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples
# for geometry files!
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/opt/spack/linux-scientific7-x86_64_v2/gcc-9.3.0/icarusalg-09.37.02.01-tyt7hh6cyhthdx5ut6ngugcrdybpgclw/gdml
# for pandora xml file
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_0/.spack-env/view/fw

# collect diagnostic profiles
export MARGO_ENABLE_DIAGNOSTICS=0
export MARGO_ENABLE_PROFILING=0

# number of events to upload and process
export NUM_EVENTS=100

export BASEDIR=$(pwd)
export THREADS=4

# start the hepnos server
echo "%%% before starting HEPnOS server for run with with $THREADS threads, at $(date)"
# 2 nodes for server, 8 MPI ranks per node
mpiexec -iface ib0 -f hostfile_server -np 16 bedrock ofi+tcp -c hepnos.json -v trace &> server-log &
sleep 30
hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json
echo "%%% after starting HEPnOS server for run with with $THREADS threads, at $(date)"

# Create queues
echo "%%% before creating queues for run with with $THREADS threads, at $(date)"
${ICARUSWF_BUILD}/src/modules/cheesyQueue_maker ofi+tcp connection.json DetSim HitFinding
echo "%%% after creating queues for run with with $THREADS threads, at $(date)"

# Load Data
echo "%%% before loading data, at $(date)"
# 5 nodes for client, 20 MPI ranks per node
mpiexec -iface ib0 -f hostfile_client -np 100 ${ICARUSWF_BUILD}/src/modules/mpi_wrapper -l --num_evts_per_rank 1 --root_file_path /wclustre/fwk/sajid/detsim_00.root &> load_00_log
mpiexec -iface ib0 -f hostfile_client -np 100 ${ICARUSWF_BUILD}/src/modules/mpi_wrapper -l --num_evts_per_rank 1 --root_file_path /wclustre/fwk/sajid/detsim_01.root &> load_01_log
echo "%%% after loading data, at $(date)"

# Process Data 
echo "%%% before processing data $(date)"
# 5 nodes for client, 8 MPI ranks per node, 4 threads per MPI rank, 5 events per MPI rank
mpiexec -iface ib0 -f hostfile_client -np 40 ${ICARUSWF_BUILD}/src/modules/mpi_wrapper -p -t 4 --num_evts_per_rank 5 &> process_log
echo "%%% after processing data $(date)"

# Shutdown the HEPnOS server
echo "%%% before shutting down HEPnOS server for run with $THREADS threads, at $(date)"
hepnos-shutdown ofi+tcp connection.json
echo "%%% after shutting down HEPnOS server for run with $THREADS threads, at $(date)"
