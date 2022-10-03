#include "mpicpp.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

bool loadData(int my_rank, int nevents){
  std::string nskip = " --nskip ";
  nskip += std::to_string(my_rank*nevents);
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string cmd = "art -c storedata.fcl -s /scratch/cerati/icaruscode-v09_37_01_02p02/icaruscode-09_37_01_02p02-samples/prodcorsika_bnb_genie_protononly_overburden_icarus_20220118T213827-GenBNBbkgr_100evt_G4_DetSim.root " + nevts + nskip;
  std::system(cmd.c_str());
  return true;
}

bool runSP(int my_rank, int nevents){
  std::string nskip = " --nskip ";
  nskip += std::to_string(my_rank*nevents);
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string memdb = " --memcheck-db memory_sp_" + std::to_string(my_rank) + ".db";
  std::string cmd = "art -c sp_hepnos.fcl --timing-db timing_sp_" + std::to_string(my_rank) + ".db" + memdb + nevts + nskip;
  std::system(cmd.c_str());
  return true;
}

int main(int argc, char *argv[]) {
  Mpi world(argc, argv);
  auto my_rank = world.rank();
  auto nranks = world.np();
  int events = std::atoi(argv[1]); 
  std::cout << "Loading data by rank "<< my_rank << " of " << nranks << std::endl;
  loadData(my_rank, events);
  world.barrier();
  runSP(my_rank, events);
  return 0;
}


