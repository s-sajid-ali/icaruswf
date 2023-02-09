#include "mpicpp.hpp"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <tclap/CmdLine.h>
#include <unistd.h>

bool
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
  std::system(cmd.c_str());
  return true;
}

bool
runSP(int my_rank,
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
  std::system(cmd.c_str());
  return true;
}

struct Config {
  bool valid{false};
  int64_t nevents{-1};
  int64_t nthreads{-1};
  bool load{false};
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
      "P",
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

    TCLAP::SwitchArg loadSwitch(
      "l",
      "load_events",
      "Whether to load events to the HEPnOS datastore",
      cmd,
      false);

    TCLAP::SwitchArg processSwitch(
      "p",
      "process_events",
      "Whether to process events in the HEPnOS datastore",
      cmd,
      false);

    // Parse the argv array.
    cmd.parse(argc, argv);
    config.valid = true;
    config.nevents = nevtsArg.getValue();
    config.load = loadSwitch.getValue();
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


int
main(int argc, char* argv[])
{
  Config config = make_config(argc, argv);
  if (!config.valid) return 1;
  // either we will configure this to run the data loading function or event
  // processing For data loading
  Mpi world(argc, argv);
  auto my_rank = world.rank();
  auto nranks = world.np();

  int64_t events = config.nevents;
  bool load_events_to_hepnos = config.load;
  std::string root_file_path = config.loadPath;
  if (load_events_to_hepnos && root_file_path.empty()) {
      std::cerr << "please provide the path to the root file to load events from!\n";
      return 1;
  }
  bool process_events_in_hepnos = config.process;
  int64_t nthreads = config.nthreads;

  // Confidence check regarding execution resources being used
  constexpr size_t LEN = 1024;
  char hostname[LEN];
  if (gethostname(hostname, LEN) == 0) {
    std::cout << "MPI rank " << my_rank << " of total " << nranks << " ranks is running on " << hostname << '\n';
  } else {
    std::cout << "could not determine execution resources being used!"
              << "\n";
  }
  std::optional<std::string> env_flags;
  //if (std::find(hostname, hostname+LEN, 
  std::string host{hostname};
  if (host.find("theta") != std::string::npos) {
    env_flags.emplace("PMI_NO_PREINITIALIZE=1 PMI_NO_FORK=1");
  }

  if (load_events_to_hepnos) {
    if (loadData(my_rank, events, root_file_path, env_flags) != true) {
      std::runtime_error("loading data into HEPnOS failed!");
    }
  }

  // is this necessary?
  world.barrier();

  if (process_events_in_hepnos) {
    if (runSP(my_rank, events, nthreads, env_flags) != true) {
      std::runtime_error("processing data in HEPnOS failed!");
    }
  }

}
