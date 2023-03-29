#!/bin/bash
set -euo pipefail

export MPICH_MAX_THREAD_SAFETY=multiple

# activate spack environment
. /scratch/gartung/spack/share/spack/setup-env.sh
spack load gcc@9.3.0
spack env activate icaruscode-09_37_02_vecmt04-hepnos-0_7_1
export ICARUSWF_SRC=/nashome/s/sasyed/packages/icaruswf
. ${ICARUSWF_SRC}/envvariable.sh

export ICARUSWF_BUILD=/nashome/s/sasyed/packages/icaruswf/build
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}

export DATA_DIR=/scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples
# for geometry files!
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/opt/spack/linux-scientific7-x86_64_v2/gcc-9.3.0/icarusalg-09.37.02.01-tyt7hh6cyhthdx5ut6ngugcrdybpgclw/gdml
# for pandora xml file
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/var/spack/environments/icaruscode-09_37_02_vecmt04-hepnos-0_7_1/.spack-env/view/fw

# Process Data 
echo "%%% before processing data $(date)"
# 5 nodes, 8 MPI ranks per node, 4 threads per MPI rank, 1 event per MPI rank
mpiexec -iface ib0 -f hostfile -np 40 ${ICARUSWF_BUILD}/src/modules/mpi_wrapper -R -t 4 --num_evts_per_rank 1 --root_file_path /wclustre/fwk/cerati/theta-files/detsim/detsim_00.root &> process_00_log
