-------------------------------------------
Changes to packages.yaml files required on artemis (might be different, or unneeded on another computer):

fix bzip2 issue:--

1. emacs -nw $SPACK_ROOT/etc/spack/ubuntu18.04/packages.yaml

2. in bzip2 section: remove line "buildable: False"

-----
3. The wrong system compiler version can be picked up.  Change all instances of gcc@7.4.0 to gcc@7.5.0 
-----
--------------------------------------------
