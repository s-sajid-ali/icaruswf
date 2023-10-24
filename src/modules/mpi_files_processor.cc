#include "fmt/format.h"
#include "mpi.h"

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <list>


int
run_art_for(std::string const& fcl_file, std::string const& input_file, std::string const& dirname) {
  std::string command;
  command = fmt::format("mkdir {}; pushd {}; art -c {} -s {} -o out.root > stdout.log 2> stderr.log; popd", dirname, dirname, fcl_file, input_file);
  return std::system(command.c_str());
}

std::string
calculate_input_file(std::string const& file_paths, int my_rank) {
  std::string line;
  std::ifstream in(file_paths);
  for (auto i = 0; i != my_rank; ++i) {
    in >> line;
    if (!in.good()) throw std::runtime_error("Too many MPI ranks for input file list");
  }
  in >> line;
  if (!in.good()) throw std::runtime_error("Too many MPI ranks for input file list");
  return line;
}


int
main(int argc, char* argv[])
{
  if (argc <= 2) {
    std::cerr << "Please specify the file containing the list of input files and one or more fcl files\n";
    return 1;
  }

  std::list<std::string> fcl_files(argv+2, argv+argc);
  std::string file_paths = argv[1];
  // initialize MPI after the configuration is all done and checked
  int rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    std::cerr << "MPI initialization failed\n";
    return 1;
  }
  int my_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  // get input_file name from file_paths.txt
  std::string first_input_file = calculate_input_file(file_paths, my_rank);
  std::string first_fcl_file = fcl_files.front();
  fcl_files.pop_front();
  std::string first_directory = fmt::format("{}_{}", first_fcl_file, my_rank);
  auto status = run_art_for(first_fcl_file, first_input_file, first_directory); 
  if (status != 0) {
    std::cerr << "art failed on rank: " << my_rank << " with status: " << status << '\n';
    std::terminate();
  }
  MPI_Barrier(MPI_COMM_WORLD);
  
  std::string previous_directory = first_directory;
  for (auto fcl_file: fcl_files) {
    std::string new_directory = fmt::format("{}_{}", fcl_file, my_rank);
    auto status = run_art_for(fcl_file, "../"+previous_directory+"/out.root", new_directory); 
    if (status != 0) {
      std::cerr << "art failed on rank: " << my_rank << " with status: " << status << '\n';
      std::terminate();
    }
    previous_directory = new_directory;
    MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Finalize();
}
