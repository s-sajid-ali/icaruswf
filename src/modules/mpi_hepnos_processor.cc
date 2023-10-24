#include "fmt/format.h"
#include "mpi.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

int
run_art_for(std::string const& fcl_file, int my_rank) {
  std::string dirname = fmt::format("{}_{}", fcl_file, my_rank);
  std::string command = fmt::format("mkdir {}; pushd {}; art -c {} > stdout.log 2> stderr.log; popd", dirname, dirname, fcl_file);
  return std::system(command.c_str());
}


int
main(int argc, char* argv[])
{
  std::vector<std::string> args(argv+1, argv+argc);
  if (args.empty()) {
    std::cerr << "Please specify one or more fcl files\n";
    return 1;
  }

  // initialize MPI after the configuration is all done and checked
  int rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    std::cerr << "MPI initialization failed\n";
    return 1;
  }
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  for (auto fcl_file: args) {
    auto status = run_art_for(fcl_file, my_rank); 
    if (status != 0) {
      std::cerr << "art failed on rank: " << my_rank << " with status: " << status << '\n';
      std::terminate();
    }
  }
  MPI_Finalize();
}
