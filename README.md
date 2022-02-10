
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

# Using spack environment with icarus code and hepnos to build new code on csresearch00.fnal.gov

1. clone the repository: `git clone git@github.com:HEPonHPC/icaruswf.git`

2. `export ICARUSWF_SRC=<path to cloned repository>`

3. make a build directory for out of source build and cd into it. 

4. setup spack: `source /scratch/gartung/spack/share/spack/setup-env.sh`

5. activate spack environment: `spack env activate icaruscode-hepnos`

6. update ROOT_INCLUDE_PATH, 

  `export ROOT_INCLUDE_PATH=/cvmfs/fermilab.opensciencegrid.org/packages/external/gcc/9.3.0/linux-scientific7-x86_64-gcc-4.8.5-arqo2nne2somynanmkuhpgjpx6xa53gi/include/c++/9.3.0:${ROOT_INCLUDE_PATH}`

7. set up the environment variables: source ${ICARUSWF_SRC}/envvariable.sh 

8. update PATH: `export PATH=/cvmfs/fermilab.opensciencegrid.org/packages/external/gcc/9.3.0/linux-scientific7-x86_64-gcc-4.8.5-arqo2nne2somynanmkuhpgjpx6xa53gi/bin/:$PATH`

9. run cmake in the build directory: cmake -DCMAKE_CXX_COMPILER=$(which g++) ${ICARUSWF_SRC} 

10. build the code: `make -j10`

11. update the CET_PLUGIN_PATH: `export CET_PLUGIN_PATH=${PWD}:${CET_PLUGIN_PATH}`

12. For running the merge module test, run the job: `art -c ${ICARUSWF_SRC}test/test_merge_module.fcl -s <icarus art/root file> -n 2`

12. For running the icarus workflow to store and load data from hepnos via an art job, do the following: 

14.  Start hepnos server: `mpirun -np 2 bedrock ofi+tcp -c hepnos.json &`, make sure you have hepnos.json, sample will be provided. If successful, hepnos.ssg file will be created. 

16.  `hepnos-list-databases ofi+tcp -s hepnos.ssg > connection.json` 

17.  `art -c ${ICARUSWF_SRC}/test/test_hepnosstore_module.fcl -s ../icaruswf_data/reco-6a3acf37-4e29-4080-a323-ef716c70d712.root -n 2`

19.  `art -c ${ICARUSWF_SRC}/test/test_hepnosinput_source.fcl`

# Samples and fcl files for icaruscode v09_37_01_02p02

Input root files and configuration files have been copied to theta.alcf.anl.gov.
Directory is: `/projects/HEP_on_HPC/icarus_data/icaruscode-v09_37_01_02p02/`

It contains one directory of fcl files:<br>
`ls icaruscode-09_37_01_02p02-configs/`<br>
`hitfinding.fcl  pandora.fcl  signalprocessing.fcl`

And one for root files:<br>
`ls icaruscode-09_37_01_02p02-samples/`<br>
`prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root  prodcorsika_bnb_genie_protononly_overburden_icarus_20220204T005428-GenBNBbkgr_10kevt.root`

In order to run the reduced set of test (signal processing, hitfinder, pandora) over a detsim sample of 100 events one can do:<br>
`lar -c signalprocessing.fcl -n -1 -s prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root`<br>
`lar -c hitfinding.fcl -n -1 -s prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim_*-MCstage0.root`<br>
`lar -c pandora.fcl -n -1 -s prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim_*-MCstage0_*-HitFinding.root`<br>

Please note that in oder to run on a subset of events, instead of specifying `-n -1` you can specify the number of events you want to run on, e.g. `-n 2` for 2 events.

In order to run the full chain set of test (g4, detsim, reco1, reco2) one can use the standard fcl files already shipped with the installation:<br>
g4: `cosmics_g4_icarus_sce_overburden.fcl`<br>
detsim: `standard_detsim_icarus.fcl`<br>
reco1: `stage0_multiTPC_icarus_MC.fcl`<br>
reco2: `stage1_multiTPC_icarus_gauss_MC.fcl`<br>
These can be run in sequence starting from a larger file with 10k events:
`prodcorsika_bnb_genie_protononly_overburden_icarus_20220204T005428-GenBNBbkgr_10kevt.root`

# Spack build instructions for ICARUS Code

export spack_area=`pwd`/spack_demo_1

export spack_vers=v0.16.1.fermi

export spack_inf_vers=v0_16_01

export SPACK_PYTHON=/usr/bin/python

export FORCE_UNSAFE_CONFIGURE=1

Setup area: 

0. mkdir -p $spack_area

1. cd $spack_area


Install spack infrastructure:

wget https://cdcvs.fnal.gov/redmine/projects/spack-infrastructure/repository/revisions/master/raw/bin/bootstrap
This wget may not work ... I had to login to the cdcvs.fnal.gov to get access to this... 

sh ./bootstrap $spack_area $spack_inf_vers $spack_vers


Install local instance of spack:

8. source $spack_area/setup-env.sh

 we can check now what repos were added: 

spack repo list

==> 5 package repositories.
local           /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/local
ups_to_spack    /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/ups_to_spack
scd_recipes     /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/scd_recipes
fnal_art        /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/fnal_art
builtin         /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/builtin

8. export spack_os=$(spack arch -o) 

9. Ask Marc Mengel: if it is being done inside script, if it is needed, is it larsoft packages specific and not done by g-2. 
cd $SPACK_ROOT/var/spack/repos/fnal_art && git checkout feature/cetmodules_patches

Install the right compiler, and switch to using it with spack:

10. cd $spack_area 

11. mv $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml.hide 

12. spack install gcc@8.2.0 arch=x86_64

12. mv $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml.hide $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml

13. spack compiler find --scope=site

14. spack load gcc@8.2.0

15. spack compiler find --scope=site

Ask Marc about : spack load spack-infrastructure@$spack_inf_vers, if this is what we need or is it for the case when we are distributing packages via ups

Update flags to be able to build with gcc8.2:
16. emacs -nw $SPACK_ROOT/etc/spack/compilers.yaml

17. In the part about gcc@8.2.0 change the line "flags: {}" to:

  flags: 

    cppflags: -Wno-deprecated -Wno-deprecated-declarations

17. Get recipe repo for HEPnOS

    git clone https://xgitlab.cels.anl.gov/sds/sds-repo.git $SPACK_ROOT/var/spack/repos/sds-repo

    spack repo add --scope=site $SPACK_ROOT/var/spack/repos/sds-repo

    export SPACK_REPOS=$SPACK_ROOT/var/spack/repos

18. Update spack recipe and configurations

    1. edit recipe for davix, `spack edit davix`. Replace the contents with the contents of https://raw.githubusercontent.com/spack/spack/14897df02b8b805243b1140dd0c4b0b15215e829/var/spack/repos/builtin/packages/davix/package.py. 

    update depends_on(uuid) to depends_on(libuuid)

    2. edit recipe for eigen, `spack edit eigen`, and replace the checksum of 3.3.5 to 0208f8d3d5f3b9fd1a022c5f47d6ebbd6f11c4ed6e756764871bf4ffb1e117a1.    
    
    3. scd_recipes/packages/gfal2/package.py:    depends_on('davix +thirdparty')

    4. edit recipe for libjpeg, update checksum for '9c' to 1e9793e1c6ba66e7e0b6e5fe7fd0f9e935cc697854d5737adec54d93e5b3f730.  
    
    5. spack edit dk2nugenie, and remove the build override, and same for dknudata

    6. spack edit nutools, and remove ~data dependency ... depends_on('geant4 ~data')

18. Create spack environment.
    
    cp /pnfs/hlml/persistent/ssehrish/icaruscode_spec_08.50.02 .

    spack env create icaruswf-dev-001 

    spack env activate icaruswf-dev-001

    spack add $(cat icaruscode_spec_08.50.02) hepnos@0.4.2%gcc@8.2.0 cxxstd=17 ^mpich@3.4.1 ^libfabric fabrics=tcp,rxm ^boost@1.70.0+serialization cxxstd=17 

19. Build and install icaruscode and hepnos:

    add concretization: together in the $SPACK_ROOT/var/spack/environments/icaruswf-dev-019/spack.yaml file, the environment number may vary. This tells spack to concretize both icaruscode and hepnos together. 
    also in spack.yaml add gcc8.2.0 explicitly for mysql, %gcc@8.2.0 to xrootd also,

    in davix recipe turn OFF, i.e. DENABLE_THIRD_PARTY_COPY=OFF

    spack concretize -f 2>&1 | tee concrete21.log 

    spack --insecure install --dirty --no-checksum 
 
-------------------------------------------
Changes to packages.yaml files required on artemis (might be different, or unneeded on another computer):

fix bzip2 issue:--

1. emacs -nw $SPACK_ROOT/etc/spack/ubuntu18.04/packages.yaml

2. in bzip2 section: remove line "buildable: False"

-----
3. The wrong system compiler version can be picked up.  Change all instances of gcc@7.4.0 to gcc@7.5.0 
-----
--------------------------------------------
