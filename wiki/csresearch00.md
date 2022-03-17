# Using spack environment with icarus code and hepnos to build new code on csresearch00.fnal.gov

1. clone the repository: `git clone git@github.com:HEPonHPC/icaruswf.git`

2. `export ICARUSWF_SRC=<path to cloned repository>`

3. make a build directory for out of source build and cd into it. 

4. setup spack: `source /scratch/gartung/spack/share/spack/setup-env.sh`

5. load the compiler via `spack load gcc@9.3.0`; activate spack environment, the current one available on csresearch is: `spack env activate icaruscode-09_37_01_02_p02-hepnos`

6. set up the environment variables: `source ${ICARUSWF_SRC}/envvariable.sh`

7. run cmake in the build directory: `cmake -DCMAKE_CXX_COMPILER=$(which g++) ${ICARUSWF_SRC}`

10. build the code: `make -j10 all`

11. Update the CET_PLUGIN_PATH: `export CET_PLUGIN_PATH=${PWD}:${CET_PLUGIN_PATH}`

13. Update the FHICL_FILE_PATH: `export FHICL_FILE_PATH=${ICARUSWF_SRC}/fcl:${FHICL_FILE_PATH}`

12. For running the icarus workflow to store and load data from hepnos via an art job, do the following: 

14.  Start hepnos server: `mpirun -np 2 bedrock ofi+tcp -c hepnos.json &`, make sure you have hepnos.json, sample will be provided. If successful, hepnos.ssg file will be created. 

16.  `hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json` 

17.  Store data (raw::RawDigits) to hepnos for the first step, signal processing; `art -c storedata.fcl -s /scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root -n 1`

19.  Run Signal processing step: `art -c sp_hepnos.fcl`

21.  Run Hit finding step: `art -c hf_hepnos.fcl`

22.  Run Pandora step: `art -c p_hepnos.fcl`

21.  If we want to write output of Pandora step to a root file, run `art -c p_hepnos_root.fcl`

