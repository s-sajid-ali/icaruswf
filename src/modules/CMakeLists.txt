configure_file(icaruswf_config.h.in icaruswf_config.h)

add_library(MergeDataProds_module SHARED MergeDataProds_module.cc)
target_link_libraries(MergeDataProds_module PRIVATE Boost::system TBB::tbb)
target_include_directories(MergeDataProds_module PRIVATE $ENV{ROOT_INC} $ENV{ART_INC} $ENV{CANVAS_INC} $ENV{CETLIB_EXCEPT_INC} $ENV{CETLIB_INC} $ENV{FHICLCPP_INC} $ENV{HEP_CONCURRENCY_INC} $ENV{LARCOREOBJ_INC} $ENV{LARDATAOBJ_INC})

#add_library(HepnosStoreData_module SHARED HepnosStoreData_module.cc)
#target_link_libraries(HepnosStoreData_module PRIVATE Boost::system hepnos Boost::serialization TBB::tbb)
#target_include_directories(HepnosStoreData_module PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{CANVAS_INC} $ENV{CETLIB_EXCEPT_INC} $ENV{CETLIB_INC} $ENV{FHICLCPP_INC} $ENV{HEP_CONCURRENCY_INC} $ENV{LARCOREOBJ_INC} $ENV{LARDATAOBJ_INC} $ENV{THALLIUM_INC} $ENV{CEREAL_INC} ${CMAKE_CURRENT_BINARY_DIR})

#add_library(HepnosStoreDataNew_module SHARED HepnosStoreDataNew_module.cc)
#target_link_libraries(HepnosStoreDataNew_module PRIVATE Boost::system hepnos Boost::serialization TBB::tbb)
#target_include_directories(HepnosStoreDataNew_module PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{CANVAS_INC} $ENV{CETLIB_EXCEPT_INC} $ENV{CETLIB_INC} $ENV{FHICLCPP_INC} $ENV{HEP_CONCURRENCY_INC} $ENV{LARCOREOBJ_INC} $ENV{LARDATAOBJ_INC} $ENV{THALLIUM_INC} $ENV{CEREAL_INC} ${CMAKE_CURRENT_BINARY_DIR})

add_library(HepnosInput_source SHARED HepnosInput_source.cc)
target_link_libraries(HepnosInput_source PRIVATE Boost::system hepnos Boost::serialization TBB::tbb art::Framework_IO_Sources $ENV{LARDATAOBJ_LIB}/liblardataobj_RecoBase.so)
target_include_directories(HepnosInput_source PRIVATE $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{LARDATAOBJ_INC} $ENV{LARCOREOBJ_INC} $ENV{THALLIUM_INC} $ENV{CEREAL_INC} ${CMAKE_CURRENT_BINARY_DIR})

#add_library(HepnosOutput_module SHARED HepnosOutput_module.cc)
#target_link_libraries(HepnosOutput_module PRIVATE Boost::system hepnos Boost::serialization TBB::tbb)
#target_include_directories(HepnosOutput_module PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{CANVAS_INC} $ENV{CETLIB_INC} $ENV{CETLIB_EXCEPT_INC} $ENV{FHICLCPP_INC} $ENV{HEP_CONCURRENCY_INC} $ENV{LARDATAOBJ_INC} $ENV{THALLIUM_INC} $ENV{CEREAL_INC} $ENV{LARCOREOBJ_INC} ${CMAKE_CURRENT_BINARY_DIR})

add_library(HepnosSPOutput_module SHARED HepnosSPOutput_module.cc)
target_link_libraries(HepnosSPOutput_module PRIVATE ROOT::Core Boost::system hepnos Boost::serialization TBB::tbb spdlog::spdlog)
target_include_directories(HepnosSPOutput_module PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{CANVAS_INC} $ENV{CETLIB_INC} $ENV{CETLIB_EXCEPT_INC} $ENV{FHICLCPP_INC} $ENV{HEP_CONCURRENCY_INC} $ENV{LARDATAOBJ_INC} $ENV{THALLIUM_INC} $ENV{CEREAL_INC} $ENV{LARCOREOBJ_INC} ${CMAKE_CURRENT_BINARY_DIR})

#add_library(HepnosDataStore_service SHARED HepnosDataStore_service.cc)
#target_link_libraries(HepnosDataStore_service PRIVATE Boost::system hepnos Boost::serialization TBB::tbb art::Framework_Services_Registry)
#target_include_directories(HepnosDataStore_service PRIVATE  $ENV{ROOT_INC} $ENV{ART_INC} $ENV{HEPNOS_INC} $ENV{THALLIUM_INC} $ENV{LARDATAOBJ_INC} $ENV{CEREAL_INC} ${CMAKE_CURRENT_BINARY_DIR})
