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
find_package(canvas REQUIRED)
find_package(hep_concurrency REQUIRED)
find_package(larcoreobj REQUIRED)
find_package(lardataobj REQUIRED)

add_subdirectory (src)

add_subdirectory(test)
