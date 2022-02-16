## Spack build instructions for ICARUS Code

Prepare to install spack-infrastructure: 

```
export spack_area=`pwd`/spack_demo_1
export spack_vers=v0.16.1.fermi
export spack_inf_vers=v0_16_01
export SPACK_PYTHON=/usr/bin/python
export FORCE_UNSAFE_CONFIGURE=1
```

Setup area: 
```
mkdir -p $spack_area
cd $spack_area
```

Install spack-infrastructure:
```
wget https://raw.githubusercontent.com/marcmengel/spack-infrastructure/master/bin/bootstrap
sh ./bootstrap $spack_area $spack_inf_vers $spack_vers
```

Add spack commands to shell & set `spack_os`:
```
source $spack_area/setup-env.sh
export spack_os=$(spack arch -o) 
```

List repositories that spack knows of:
```
spack repo list

==> 5 package repositories.
local           /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/local
ups_to_spack    /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/ups_to_spack
scd_recipes     /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/scd_recipes
fnal_art        /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/fnal_art
builtin         /work1/fwk/icaruswfgoo/spack_demo_1/spack/v0.16.1.fermi/NULL/var/spack/repos/builtin
```

#### TODO: Ask Marc Mengel: if it is being done inside script, if it is needed, is it larsoft packages specific and not done by g-2. 
```
cd $SPACK_ROOT/var/spack/repos/fnal_art && git checkout feature/cetmodules_patches
```

Install the right compiler, and switch to using it with spack:
```
cd $spack_area 
mv $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml.hide 
spack install gcc@8.2.0 arch=x86_64
mv $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml.hide $SPACK_ROOT/etc/spack/${spack_os}/packages.yaml
spack compiler find --scope=site
spack load gcc@8.2.0
spack compiler find --scope=site
```

#### TODO Ask Marc about : spack load spack-infrastructure@$spack_inf_vers, if this is what we need or is it for the case when we are distributing packages via ups

Update flags to be able to build with gcc8.2:
```
emacs -nw $SPACK_ROOT/etc/spack/compilers.yaml
```

In the part about gcc@8.2.0 change the line "flags: {}" to:
```
  flags: 

    cppflags: -Wno-deprecated -Wno-deprecated-declarations
```

Get recipe repo for HEPnOS
```
git clone github.com/mochi-hpc/mochi-spack-packages/ $SPACK_ROOT/var/spack/repos/nmochi-repo
spack repo add --scope=site $SPACK_ROOT/var/spack/repos/mochi-repo
export SPACK_REPOS=$SPACK_ROOT/var/spack/repos
```

Update spack recipe and configurations
 - edit recipe for davix, `spack edit davix`. Replace the contents with the contents of https://raw.githubusercontent.com/spack/spack/14897df02b8b805243b1140dd0c4b0b15215e829/var/spack/repos/builtin/packages/davix/package.py. 
 - update depends_on(uuid) to depends_on(libuuid)
 - edit recipe for eigen, `spack edit eigen`, and replace the checksum of 3.3.5 to 0208f8d3d5f3b9fd1a022c5f47d6ebbd6f11c4ed6e756764871bf4ffb1e117a1.    
 - scd_recipes/packages/gfal2/package.py:    depends_on('davix +thirdparty')
 - edit recipe for libjpeg, update checksum for '9c' to 1e9793e1c6ba66e7e0b6e5fe7fd0f9e935cc697854d5737adec54d93e5b3f730.  
 - spack edit dk2nugenie, and remove the build override, and same for dknudata
 - spack edit nutools, and remove ~data dependency ... depends_on('geant4 ~data')

Create spack environment.
```
cp /pnfs/hlml/persistent/ssehrish/icaruscode_spec_08.50.02 .
spack env create icaruswf-dev-001 
spack env activate icaruswf-dev-001
spack add $(cat icaruscode_spec_08.50.02) hepnos@0.4.2%gcc@8.2.0 cxxstd=17 ^mpich@3.4.1 ^libfabric fabrics=tcp,rxm ^boost@1.70.0+serialization cxxstd=17 
```

Build and install icaruscode and hepnos:

- add concretization: together in the $SPACK_ROOT/var/spack/environments/icaruswf-dev-019/spack.yaml file, the environment number may vary. This tells spack to concretize both icaruscode and hepnos together. 
- also in spack.yaml add gcc8.2.0 explicitly for mysql, %gcc@8.2.0 to xrootd also,
- in davix recipe turn OFF, i.e. DENABLE_THIRD_PARTY_COPY=OFF

```
spack concretize -f 2>&1 | tee concrete21.log 
spack --insecure install --dirty --no-checksum 
```
