#!/bin/bash
set -euo pipefail

# activate spack environment
. /scratch/gartung/spack/share/spack/setup-env.sh
spack load gcc@9.3.0
spack env activate icaruscode-09_37_01_03_p02-hepnos
#spack env activate icaruscode-09_37_02_03-hepnos

export ICARUSWF_SRC=/nashome/s/sasyed/packages/icaruswf
. ${ICARUSWF_SRC}/envvariable.sh

export ICARUSWF_BUILD=/nashome/s/sasyed/packages/icaruswf/build
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}

export DATA_DIR=/scratch/cerati/icaruscode-v09_37_01_03p02/icaruscode-09_37_01_03p02-samples
# for geometry files!
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/opt/spack/linux-scientific7-x86_64_v2/gcc-9.3.0/icarusalg-09.37.02.01-24uyymplbwvalzi36hbosaumhvlnp4ql/gdml
export FW_SEARCH_PATH=${FW_SEARCH_PATH}:/scratch/gartung/spack/opt/spack/linux-scientific7-x86_64_v2/gcc-9.3.0/icaruscode-09.37.01.03p02-jtcu4f5sckuediz2d4je54snhfwbylhg/fw

export BASEDIR=$(pwd)
# Create all the directories!
for THREADS in 1
do

  cd $BASEDIR
  mkdir -p threads_$THREADS
  cd threads_$THREADS

  for RUN in {1..5}
  do
    mkdir -p run_$RUN
  done
done
cd $BASEDIR

# run the benchmark
for RUN in 1
do
  # run icaruswf-bench with varying number of client threads
  for THREADS in 1
  do

      cd ${BASEDIR}/threads_$THREADS/run_$RUN
      export NUM_CLIENT_THREADS_PER_RANK=$THREADS

      echo "%%% before icaruswf-root-sigproc with $THREADS threads, run number $RUN at $(date)"
      art --nschedules 1 --nthreads ${NUM_CLIENT_THREADS_PER_RANK} -c sp_root.fcl -s /scratch/ssehrish/icaruswf_build/test/test-ds.root -T /dev/null &> sp_out
      echo "%%% after icaruswf-root-sigproc with $THREADS threads, run number $RUN at $(date)"

      echo "%%% before icaruswf-root-hit-find with $THREADS threads, run number $RUN at $(date)"
      infile=$(find .  -name "test*SignalProcessing\.root")
      art --nschedules 1 --nthreads ${NUM_CLIENT_THREADS_PER_RANK} -c hf_root.fcl -s $infile -T /dev/null &> hf_out
      echo "%%% after icaruswf-root-hit-find with $THREADS threads, run number $RUN at $(date)"

      echo "%%% before icaruswf-root-pandora with $THREADS threads, run number $RUN at $(date)"
      infile=$(find .  -name "test*HitFinding\.root")
      art --nschedules 1 --nthreads ${NUM_CLIENT_THREADS_PER_RANK} -c p_root.fcl -s $infile -T /dev/null &> p_out
      echo "%%% after icaruswf-root-pandora with $THREADS threads, run number $RUN at $(date)"

  done
done
