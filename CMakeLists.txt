cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(icaruswf LANGUAGES CXX C)

enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(MPI REQUIRED COMPONENTS C)
find_package(Boost REQUIRED COMPONENTS system serialization)
find_package(hepnos REQUIRED)
find_package(ROOT REQUIRED)
find_package(TBB REQUIRED)
find_package(art REQUIRED)
find_package(larcoreobj REQUIRED)
find_package(lardataobj REQUIRED)

add_library(MergeDataProds_module SHARED MergeDataProds_module.cc)
target_link_libraries(MergeDataProds_module PRIVATE Boost::system TBB::tbb)
target_include_directories(MergeDataProds_module PRIVATE $ENV{ROOT_INC} $ENV{ART_INC})

add_library(HepnosStoreData_module SHARED HepnosStoreData_module.cc)
target_link_libraries(HepnosStoreData_module PRIVATE Boost::system hepnos Boost::serialization TBB::tbb)
target_include_directories(HepnosStoreData_module PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC})

add_library(HepnosInput_source SHARED HepnosInput_source.cc)
target_link_libraries(HepnosInput_source PRIVATE Boost::system hepnos Boost::serialization TBB::tbb art::Framework_IO_Sources $ENV{LARDATAOBJ_LIB}/liblardataobj_RecoBase.so)
target_include_directories(HepnosInput_source PRIVATE $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{LARDATAOBJ_INC})

add_subdirectory(test)

