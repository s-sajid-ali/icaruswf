# Using spack environment with icarus code and hepnos to build new code on csresearch00.fnal.gov

1. clone the repository: `git clone git@github.com:HEPonHPC/icaruswf.git`

2. `export ICARUSWF_SRC=<path to cloned repository>`

3. make a build directory for out of source build and cd into it. 

4. setup spack: `source /scratch/gartung/spack/share/spack/setup-env.sh`

5. load the compiler via `spack load gcc@9.3.0`; activate spack environment: `spack env activate icaruscode-hepnos`

6. update ROOT_INCLUDE_PATH, 

  `export ROOT_INCLUDE_PATH=/cvmfs/fermilab.opensciencegrid.org/packages/external/gcc/9.3.0/linux-scientific7-x86_64-gcc-4.8.5-arqo2nne2somynanmkuhpgjpx6xa53gi/include/c++/9.3.0:${ROOT_INCLUDE_PATH}`

7. set up the environment variables: `source ${ICARUSWF_SRC}/envvariable.sh`

8. update PATH: `export PATH=/cvmfs/fermilab.opensciencegrid.org/packages/external/gcc/9.3.0/linux-scientific7-x86_64-gcc-4.8.5-arqo2nne2somynanmkuhpgjpx6xa53gi/bin/:$PATH`

9. run cmake in the build directory: `cmake -DCMAKE_CXX_COMPILER=$(which g++) ${ICARUSWF_SRC}`

10. build the code: `make -j10 all`

11. update the CET_PLUGIN_PATH: `export CET_PLUGIN_PATH=${PWD}:${CET_PLUGIN_PATH}`

12. For running the merge module test, run the job: `art -c ${ICARUSWF_SRC}test/test_merge_module.fcl -s <icarus art/root file> -n 2`

12. For running the icarus workflow to store and load data from hepnos via an art job, do the following: 

14.  Start hepnos server: `mpirun -np 2 bedrock ofi+tcp -c hepnos.json &`, make sure you have hepnos.json, sample will be provided. If successful, hepnos.ssg file will be created. 

16.  `hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json` 

17.  `art -c ${ICARUSWF_SRC}/test/test_hepnosstore_module.fcl -s ../icaruswf_data/reco-6a3acf37-4e29-4080-a323-ef716c70d712.root -n 2`

19.  `art -c ${ICARUSWF_SRC}/test/test_hepnosinput_source.fcl`

