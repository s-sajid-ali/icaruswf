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
 
