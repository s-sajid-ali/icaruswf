
# ICARUS-worflow

This repository contains art modules and services for using the HEPnOS data service as part of the ICARUS workflow. 

Implemented art services: 
 - HepnosDataStore

Implemented art modules: 
 - Input module    : HepnosInputSource
 - Output module   : HepnosOutput
 - Shared analyzer : HepnosStoreData(New)

As is the case with art, one needs a set of FHiCL files to define configurations for a workflow. A set of samples for `v09_37_01_02p02` can be found [here](wiki/v09370102p02.md).

Generic build instructions using the spack package manager can be found [here](wiki/spackbuild.md)

### Instructions to build and run on: 
 - [ALCF-theta](wiki/theta.md)
 - [FNAL-wc-csresearch00](wiki/csresearch00.md)

Extra miscellaneous information can be found [here](wiki/misc.md)
