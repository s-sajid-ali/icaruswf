#include "mpicpp.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <tclap/Arg.h>
#include <tclap/CmdLine.h>
#include <unistd.h>

struct Config {
  bool valid() const{
   // This logic needs to be more robust, 
   // the configuration is valid if either load path or process flag is specified not both. 
    return (process == loadPath.empty());
  };
  int64_t nevents{-1};
  int64_t nthreads{-1};
  std::string loadPath;
  bool process{false};
};

Config
make_config(int argc, char* argv[])
{

  Config config;
  try {
    TCLAP::CmdLine cmd(
      "ICARUSWF with HEPnOS mpi-wrapper for launching art over MPI!");

    TCLAP::ValueArg<int64_t> nevtsArg(
      "n",
      "num_evts_per_rank",
     "total number of events to process, by each MPI rank",
      true,
      0,
      "int64");
    cmd.add(nevtsArg);

    TCLAP::ValueArg<std::string> rootfilepathArg(
      "l",
      "root_file_path",
      "location of the ROOT file to process",
      false,
      "",
      "string");
    cmd.add(rootfilepathArg);

    TCLAP::ValueArg<int64_t> threadsArg(
      "t",
      "threads",
      "number of threads to use for SH processing",
      false,
      1,
      "int64");
    cmd.add(threadsArg);

    TCLAP::SwitchArg processSwitch(
      "p",
      "process_events",
      "Whether to process events in the HEPnOS datastore",
      false);
    cmd.add(processSwitch);
    // Parse the argv array.
    cmd.parse(argc, argv);
    config.nevents = nevtsArg.getValue();
    config.loadPath = rootfilepathArg.getValue();
    config.process = processSwitch.getValue();
    config.nthreads = threadsArg.getValue();
  }
  catch (TCLAP::ArgException& e) // catch exceptions
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId()
              << std::endl;
  }
  return config;
}

// the fcl file for loading shouldn't be hard coded, 
// for now its okay but should be fixed at some point

int
loadData(int my_rank,
         int nevents,
         std::string path,
         std::optional<std::string> env_flags)
{
  std::string nskip = " --nskip ";
  nskip += std::to_string(my_rank * nevents);
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string cmd = "";
  if (env_flags.has_value()) {
    cmd.append(env_flags.value());
  }
  cmd.append("art -c storedata_queue.fcl -s ");
  cmd.append(path);
  cmd.append(nevts);
  cmd.append(nskip);
  std::string outfile = " &> ";
  outfile.append(std::to_string(my_rank));
  outfile.append("_load.txt");
  cmd.append(outfile);
  return std::system(cmd.c_str());
}

// this is to run the signal processing and hitfinding steps
// as specified in the fcl file. the name is hard coded here 
// as well, may be we want to change it as well
int
runSH(int my_rank,
      int nevents,
      int nthreads,
      std::optional<std::string> env_flags)
{
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string memdb =
    " --memcheck-db memory_sp_" + std::to_string(my_rank) + ".db";

  std::string cmd = "";
  if (env_flags.has_value()) {
    cmd.append(env_flags.value());
  }
  cmd.append("art --nschedules 1 --nthreads " + std::to_string(nthreads) +
             " -c icarus_SH_queue.fcl "
             "--timing-db timing_sp_" +
             std::to_string(my_rank) + ".db" + memdb + nevts);
  std::string outfile = " &> ";
  outfile.append(std::to_string(my_rank));
  outfile.append("_process.txt");
  cmd.append(outfile);
  return std::system(cmd.c_str());
}

//retunr hostname along with 
//printout of rank and total ranks
//the printout is only relevant for debugging
std::string
get_hostname(int my_rank, 
               int nranks)
{
  constexpr size_t LEN = 1024;
  char host[LEN];
  if (gethostname(host, LEN) == 0) {
    std::cout << "MPI rank " << my_rank << " of total " << nranks << " ranks is running on " << host << '\n';
  } else {
    std::cout << "could not determine execution resources being used!\n";
  }
  std::string hostname{host};
  return hostname;
}

int
main(int argc, char* argv[])
{
  Config config = make_config(argc, argv);
  // either we will configure this to run the data loading function or event
  // processing For data loading
  if (!config.valid()) {
    std::cerr << "invalid configuration\n";
    return 1;
  }  

  int64_t events = config.nevents;
  std::string root_file_path = config.loadPath;
  bool load_events_to_hepnos = !root_file_path.empty();
  bool process_events_in_hepnos = config.process;
  if (!process_events_in_hepnos && root_file_path.empty()) {
      std::cerr << "please provide the path to the root file to load events from!\n";
      return 1;
  }
  int64_t nthreads = config.nthreads;

  //initialize MPI after the configuration is all done and checked
  Mpi world(argc, argv);
  auto my_rank = world.rank();
  auto nranks = world.np();

  // Confidence check regarding execution resources being used
  // nranks and my_rank are only used to print out the rank information
  // they are not needed for getting the hostname
  auto hostname = get_hostname(nranks, my_rank);
  
  // Set special flags for running on Theta
  std::optional<std::string> env_flags;   
  if (hostname.find("theta") != std::string::npos) {
    env_flags.emplace("PMI_NO_PREINITIALIZE=1 PMI_NO_FORK=1");
  }
  
  // finally either load events
  if (load_events_to_hepnos) {
    return loadData(my_rank, events, root_file_path, env_flags);
  }
  // OR process events
  if (process_events_in_hepnos) {
    return runSH(my_rank, events, nthreads, env_flags);
  }
}
