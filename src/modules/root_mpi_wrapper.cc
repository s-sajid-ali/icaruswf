#include "mpicpp.hpp"
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <stdexcept>
#include <string>
#include <tclap/Arg.h>
#include <tclap/CmdLine.h>
#include <unistd.h>

struct Config {
  int64_t nevents{-1};
  int64_t nthreads{-1};
  std::string loadPath;
};

Config
make_config(int argc, char* argv[])
{

  Config config;
  try {
    TCLAP::CmdLine cmd(
      "ICARUSWF with ROOT mpi-wrapper for launching art over MPI!");

    TCLAP::ValueArg<int64_t> nevtsArg(
      "n",
      "num_evts_per_rank",
      "total number of events to process, by each MPI rank",
      true,
      0,
      "int64");
    cmd.add(nevtsArg);

    TCLAP::ValueArg<std::string> rootfilepathArg(
      "f",
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

    // Parse the argv array.
    cmd.parse(argc, argv);
    config.nevents = nevtsArg.getValue();
    config.loadPath = rootfilepathArg.getValue();
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
runSP(int my_rank,
      int nevents,
      int nthreads,
      std::string path,
      std::optional<std::string> env_flags)
{
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string memdb =
    " --memcheck-db memory_sp_" + std::to_string(my_rank) + ".db";
  std::string timedb =
    " --timing-db timing_sp_" + std::to_string(my_rank) + ".db";

  std::string cmd = "";
  if (env_flags.has_value()) {
    cmd.append(env_flags.value());
  }
  cmd.append("art --nschedules 1 --nthreads " + std::to_string(nthreads) +
             " -c sp_root.fcl " + timedb + memdb + nevts + " -s ");
  cmd.append(path);
  std::string outfile = " &> ";
  outfile.append(std::to_string(my_rank));
  outfile.append("_sp.txt");
  cmd.append(outfile);
  return std::system(cmd.c_str());
}

int
runHF(int my_rank,
      int nevents,
      int nthreads,
      std::optional<std::string> env_flags)
{
  std::string nevts = " -n ";
  nevts += std::to_string(nevents);
  std::string memdb =
    " --memcheck-db memory_hf_" + std::to_string(my_rank) + ".db";
  std::string timedb =
    " --timing-db timing_hf_" + std::to_string(my_rank) + ".db";

  std::string cmd = "";
  if (env_flags.has_value()) {
    cmd.append(env_flags.value());
  }
  cmd.append("art --nschedules 1 --nthreads " + std::to_string(nthreads) +
             " -c hf_root.fcl " + timedb + memdb + nevts +
             " -s sp_output.root");
  std::string outfile = " &> ";
  outfile.append(std::to_string(my_rank));
  outfile.append("_sp.txt");
  cmd.append(outfile);
  return std::system(cmd.c_str());
}

// retunr hostname along with
// printout of rank and total ranks
// the printout is only relevant for debugging
std::string
get_hostname(int my_rank, int nranks)
{
  constexpr size_t LEN = 1024;
  std::string hostname;
  hostname.resize(LEN);
  if (gethostname(hostname.data(), LEN) == 0) {
    std::cout << "MPI rank " << my_rank << " of total " << nranks
              << " ranks is running on " << host << '\n';
  } else {
    std::cout << "could not determine execution resources being used!\n";
  }
  return hostname;
}

int
main(int argc, char* argv[])
{
  Config config = make_config(argc, argv);

  int64_t events = config.nevents;
  std::string root_file_path = config.loadPath;
  if (root_file_path.empty()) {
    std::cerr
      << "please provide the path to the root file to load events from!\n";
    return 1;
  }
  int64_t nthreads = config.nthreads;

  // initialize MPI after the configuration is all done and checked
  Mpi world(argc, argv);
  auto my_rank = world.rank();
  auto nranks = world.np();

  // Confidence check regarding execution resources being used
  // nranks and my_rank are only used to print out the rank information
  // they are not needed for getting the hostname
  auto hostname = get_hostname(nranks, my_rank);

  // Set special flags for running on Theta
  std::optional<std::string> env_flags;
  // hostname on theta is just nid0000, so check that we are
  // not running on csresearch for now.
  if (hostname.find("csresearch") == std::string::npos) {
    env_flags.emplace("PMI_NO_PREINITIALIZE=1 PMI_NO_FORK=1 ");
  }

  // create a directory for this MPI rank and cd into it!
  std::filesystem::create_directory(std::to_string(my_rank));
  std::filesystem::current_path(std::to_string(my_rank));

  if (runSP(my_rank, events, nthreads, root_file_path, env_flags) != 0) {
    std::cerr << "error when running signal processing with ROOT!";
  }
  if (runHF(my_rank, events, nthreads, env_flags) != 0) {
    std::cerr << "error when running hit finding with ROOT!";
  }

  return;
}
