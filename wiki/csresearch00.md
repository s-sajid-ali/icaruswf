# Using spack environment with icarus code and hepnos to build new code on csresearch00.fnal.gov

## Establishing a new working directory 
```
# environment variables for current versions of icarus code and hepnos.
export ICARUS_VERSION=09_37_02_vecmt04
export HEPNOS_VERSION=0_7_2
export MERCURY_TRANSPORT_PROTOCOL="ofi+tcp"

# cd into the directory where you want to work and clone the repository:
export TOP_DIR=$PWD
git clone git@github.com:HEPonHPC/icaruswf.git
export ICARUSWF_SRC=${TOP_DIR}/icaruswf

# Make a build directory for out of source build and cd into it.
mkdir -p ${TOP_DIR}/icaruswf_build
export ICARUSWF_BUILD=${TOP_DIR}/icaruswf_build

# Setup spack:
source /scratch/gartung/spack/share/spack/setup-env.sh

# Load the compiler
spack load gcc@9.3.0

# activate spack environment
spack env activate icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}

# Set up the environment variables
source ${ICARUSWF_SRC}/envvariable.sh

# Run cmake in the build directory
cd ${ICARUSWF_BUILD}
cmake -DCMAKE_CXX_COMPILER=$(which g++) -DMERCURY_TRANSPORT_PROTOCOL="${MERCURY_TRANSPORT_PROTOCOL}" ${ICARUSWF_SRC} 
```

## Establishing a new working session

```
# Setup spack:
source /scratch/gartung/spack/share/spack/setup-env.sh

# Make sure to cd into the directory in which you executed the `git clone` command. 

# environment variables for current versions of icarus code and hepnos, and source and build directories.
export ICARUS_VERSION=09_37_02_vecmt04
export HEPNOS_VERSION=0_7_2
export MERCURY_TRANSPORT_PROTOCOL="ofi+tcp"
export TOP_DIR=$PWD
export ICARUSWF_SRC=${TOP_DIR}/icaruswf
export ICARUSWF_BUILD=${TOP_DIR}/icaruswf_build
export CET_PLUGIN_PATH=${ICARUSWF_BUILD}/src/modules:${CET_PLUGIN_PATH}
export FHICL_FILE_PATH=${ICARUSWF_BUILD}/fcl:${FHICL_FILE_PATH}
export FW_SEARCH_PATH=`spack location -i icaruscode`/fw:${FW_SEARCH_PATH}

# Load the compiler
spack load gcc@9.3.0

# activate spack environment
spack env activate icaruscode-${ICARUS_VERSION}-hepnos-${HEPNOS_VERSION}

# Set up the environment variables
source ${ICARUSWF_SRC}/envvariable.sh
```

## Build the code
```
cd ${ICARUSWF_BUILD}
make -j10 all
```

## An example of running icarus workflow to store and process data from hepnos using art
```
cd ${ICARUSWF_BUILD}/test

#Start hepnos server using two mpi ranks.
mpirun -np 2 bedrock "${MERCURY_TRANSPORT_PROTOCOL}" -c hepnos.json &

# If the above mpirun is successful, a hepnos.ssg file will be created. 
hepnos-list-databases "${MERCURY_TRANSPORT_PROTOCOL}" -s hepnos.ssg > connection.json 

# Store data (raw::RawDigits) to hepnos for the first step, followed by running signal processing, hit finding and  pandora. 
art -c storedata.fcl -s /scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root -n 1
art -c sp_hepnos.fcl
art -c hf_hepnos.fcl
art -c p_hepnos.fcl

# If we want to write output of Pandora step to a root file:
art -c p_hepnos_root.fcl
```

## Specifying options for input source

Three options are added for the HepnosInputSource. One is to specify number of events to read from the store using `-n/--nevts`. The nnext one is to specify the number of events to skip, `--nskip` and the last one is to specify the starting event ID in the following form: `--estart/-e`, `runnumber:subrunnumber:eventnumber`. Both nskip and estart should not be specified together. 

1. `art -c hf_hepnos.fcl -e 1:2:3 -n 1` will run hit finding step on 1 event with run number 1, subrun number 2 and event number 3. If there is no such event the read function will return false and application will exit without running any processing. 

2. `art -c hf_hepnos.fcl --nskip 2 -n 2` will skip two events and process next two events. 

## Use of the new queue API

A new input source and output modules are added; `LoadbalancingInput_source.cc` and `LoadbalancingOutput_module.cc`. Two new fcl files are added that make use of these modules, i.e. `storedata_queue.fcl` and `icarus_SH_queue.fcl`.   
1. `hepnos.json.in` is already updated to use the queues. 
2. Start hepnos server: `mpirun -np 1 bedrock ofi+tcp -c hepnos.json &`, make sure you have hepnos.json, sample will be provided. If successful, hepnos.ssg file will be created. 
3. `hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json` 
4. Run the application to create queues in the hepnos, the application name is `cheesyqueue`. From the test/ directory, we can run the application: `../src/modules/cheesyQueue_maker ofi+tcp connection.json DetSim HitFinding`. This will create two queues names DetSim and HitFinding. 
5. Store data (raw::RawDigits) to hepnos for the first step. `storedata_queue.fcl` is the fcl file with appropriate changes; `art -c storedata_queue.fcl -s /scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root -n 1`
6. Then run the next step that has both signal processing and hit finding. The appropriate fcl file to use is `icarus_SH_queue.fcl`. `art -c icarus_SH_queue.fcl -n 1`.  

## MPI wrapper application

MPI wrapper application is a simple MPI program to run multiple art instances. 
It can either launch multiple instances of art to load events Or to process the loaded events. 
1. Once the hepnos server is up and running, and queues are created, then from the `${ICARUSWF_BUILD}/test` directory, use 
`mpirun -np 2 ./../src/modules/mpi_wrapper -n 2 -l /wclustre/fwk/cerati/theta-files/detsim_00.root`, this will write 4 events in total, 2 events per MPI rank. 
2. Then run the  signal processing and hit finding on all 4 events, `mpirun -np 2 ./../src/modules/mpi_wrapper -n 2 -p`. 

