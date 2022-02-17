# Using spack environment with icarus code and hepnos to build new code on Theta 

1. clone the repository, : `git clone git@github.com:HEPonHPC/icaruswf.git`

2. Get an interactive node: `qsub -I -n 4 -t 01:00 --attrs enable_ssh=1 -q debug-cache-quad -A HEP_on_HPC`

3. `module unload PrgEnv-intel`

4. `export ICARUSWF_SRC=<path to cloned repository>` 

5. make a build directory for out of source build and cd into it.
 
6. `source /projects/HEP_on_HPC/icaruscode/spack/share/spack/setup-env.sh` 
 
7. `spack load cmake` and `spack load gcc@9.4.0`
 
8. activate spack environment: `spack env activate icaruscode-hepnos-9-37-01-02p2`
 
9. set up the environment variables: `source ${ICARUSWF_SRC}/envvariable.sh` 
 
10. update ROOT_INCLUDE_PATH, `export ROOT_INCLUDE_PATH=/lus/theta-fs0/projects/HEP_on_HPC/icaruscode/spack/opt/spack/cray-cnl7-haswell/gcc-7.3.0/gcc-9.4.0-mb3r4vv5srge7q3qh4gkbbbmsgjdv4oq/include/c++/9.4.0/:${ROOT_INCLUDE_PATH}`

11. run cmake in the build directory: `cmake -DCMAKE_CXX_COMPILER=$(which g++) ${ICARUSWF_SRC}`

12. build the code: `make -j10` 

13. update the CET_PLUGIN_PATH: `export CET_PLUGIN_PATH=${PWD}:${CET_PLUGIN_PATH}`

14. Start hepnos server: `aprun -n 2 bedrock ofi+tcp -c /projects/HEP_on_HPC/sehrish/hepnos.json &`, 

15. followed by `hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json`

16. `aprun -n 1 -N 1 art -c ${ICARUSWF_SRC}/test/test_hepnosstore_module.fcl -s ../../icarus_data/poms_icarus_prod_cosmics_v07_11_00/reco-
6a3acf37-4e29-4080-a323-ef716c70d712.root -n 2`

17. `aprun -n 1 -N 1 art -c ${ICARUSWF_SRC}/test/test_hepnosinput_source.fcl`

