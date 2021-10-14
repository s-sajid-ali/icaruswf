#export SRC=/scratch/ssehrish/icaruswf

export ART_DIR=`spack location -i art`
export ARTDAQ_CORE_DIR=`spack location -i artdaq-core`
export BOOST_DIR=`spack location -i boost`
export CETLIB_EXCEPT_DIR=`spack location -i cetlib-except`
export CETLIB_DIR=`spack location -i cetlib`
export TRACE_DIR=`spack location -i trace`
export LARDATAOBJ_DIR=`spack location -i lardataobj`
export LARCOREOBJ_DIR=`spack location -i larcoreobj`
export ROOTSYS=`spack location -i root`


export ART_INC=${ART_DIR}/include
export ARTDAQ_CORE_INC=${ARTDAQ_CORE_DIR}/include
export BOOST_INC=${BOOST_DIR}/include
export CETLIB_EXCEPT_INC=${CETLIB_EXCEPT_DIR}/include
export CETLIB_INC=${CETLIB_DIR}/include
export TRACE_INC=${TRACE_DIR}/include
export LARDATAOBJ_INC=${LARDATAOBJ_DIR}/include
export LARCOREOBJ_INC=${LARCOREOBJ_DIR}/include
export ROOT_INC=${ROOTSYS}/include

export ART_LIB=${ART_DIR}/lib
export BOOST_LIB=${BOOST_DIR}/lib
export ARTDAQ_CORE_LIB=${ARTDAQ_CORE_DIR}/lib
export CETLIB_LIB=${CETLIB_DIR}/lib
export CETLIB_EXCEPT_LIB=${CETLIB_EXCEPT_DIR}/lib
export LARDATAOBJ_LIB=${LARDATAOBJ_DIR}/lib
export LARCOREOBJ_LIB=${LARCOREOBJ_DIR}/lib

