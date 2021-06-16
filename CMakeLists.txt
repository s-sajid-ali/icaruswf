cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(icaruswf LANGUAGES CXX)

enable_testing()
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Boost REQUIRED COMPONENTS system)
#find_package(ROOT REQUIRED)
#find_package(TBB REQUIRED)
#find_package(art REQUIRED)
#find_package(larcoreobj REQUIRED)
#find_package(lardataobj REQUIRED)

add_library(MergeDataProds_module SHARED MergeDataProds_module.cc)
target_link_libraries(MergeDataProds_module PRIVATE Boost::system)
target_include_directories(MergeDataProds_module PRIVATE $ENV{TBB_INC} $ENV{ROOT_INC} $ENV{ART_INC})
add_subdirectory(test)

