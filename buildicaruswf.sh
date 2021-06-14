export SRC=/scratch/ssehrish/icaruswf

cmake ${SRC} -Dart_DIR=$(spack location -i art)/lib/art/cmake -DTBB_DIR=$(spack location -i tbb)/cmake -DCMAKE_CXX_COMPILER=$(which g++)



