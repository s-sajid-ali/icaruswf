# Using spack environment with icarus code and hepnos to build new code on Theta and run icarus workflow with hepnos using interactive session

1. clone the repository, : `git clone git@github.com:HEPonHPC/icaruswf.git`

2. Get an interactive node: `qsub -I -n 4 -t 01:00 --attrs enable_ssh=1 -q debug-cache-quad -A HEP_on_HPC`

3. `module unload PrgEnv-intel`

4. `export ICARUSWF_SRC=<path to cloned repository>` 

5. make a build directory for out of source build and cd into it. 
 
6. `source /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh` 
 
7. `spack load cmake` and `spack load gcc@9.4.0`
 
8. activate spack environment: `spack env activate icaruscode-hepnos-9-37-01-02p2`
 
9. set up the environment variables: `source ${ICARUSWF_SRC}/envvariable.sh` 
 
10. define FHICL_FILE_PATH: `export FHICL_FILE_PATH=$SPACK_ENV/.spack-env/view/fcl`

11. run cmake in the build directory: `cmake -DCMAKE_CXX_COMPILER=$(which g++) ${ICARUSWF_SRC}`

12. build the code: `make -j10` 

13. update the CET_PLUGIN_PATH: `export CET_PLUGIN_PATH=${PWD}:${CET_PLUGIN_PATH}`

13. define following before starting hepnos server: `export MPICH_GNI_NDREG_ENTRIES=1024` and `export MPICH_MAX_THREAD_SAFETY=multiple`

14. Define protection domains: `export PDOMAIN=hepnos-icarus` and `apstat -P | grep ${PDOMAIN} || apmgr pdomain -c -u ${PDOMAIN}`

15. Start hepnos server: `aprun -n 2 -p ${PDOMAIN} bedrock ofi+gni -c /projects/HEP_on_HPC/sehrish/icarus-configs/hepnos.json &`, 

16. followed by `aprun -n 1 -p ${PDOMAIN} hepnos-list-databases ofi+gni -s /projects/HEP_on_HPC/sehrish/icarus-configs/hepnos.ssg > connection.json`, there may be some extra lines at the end of connection.json, they should be removed. 

17. Store data to hepnos: `aprun -n 1 -N 1 -p ${PDOMAIN} art -c /projects/HEP_on_HPC/sehrish/icarus-configs/storedata.fcl -s /projects/HEP_on_HPC/icarus_data/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root -n 1`

18. Run signal processing, using hepnos as input and output: `aprun -n 1 -N 1 -p ${PDOMAIN} art -c /projects/HEP_on_HPC/sehrish/icarus-configs/sp_hepnos.fcl`

