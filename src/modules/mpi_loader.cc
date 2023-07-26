#include "fmt/format.h"
#include "mpi.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct art_params {
  std::string input_file;
  int nskip = 0;
  int nevents = 0;
};

struct file_record {
  std::string input_file;
  int nevents = 0;
};

std::istream & operator>>(std::istream & in, file_record & e)
{
   if (!in) return in;
   in >> e.input_file;
   if (!in) return in;
   int x;
   in >> x;
   in >> x;
   in >> e.nevents;
   if (!in) return in;
   in >> x;
   return in;
}

struct mpi_record {
  int index = 0;
};

struct skip_info {
  int nskip = 0;
  int nevents = 0;
};

std::vector<file_record>
read_file_records(std::string const& fname)
{
  std::ifstream in(fname);
  std::vector<file_record> res;
  
  while (in.good()) {
    file_record rec;
    in >> rec;
    if (in) res.push_back(rec);
  }
  return res;
}

skip_info
skip_for_rank(int my_rank, int ranks, int nevents)
{
  auto [q, r] = std::div(nevents, ranks);
  if (my_rank < r)
    return {(q + 1) * my_rank, q + 1};
  return {(q * my_rank) + r, q};
}


int 
calculate_index(std::size_t nfiles, int my_rank, int nranks)
{
  if (nfiles > nranks) {
    throw std::logic_error("Must use atleast as many ranks as files");
  }
  auto [q, r] = std::div(nranks, nfiles);
  std::vector<int> ranks_for_file(nfiles, q);
  for ( int i = 0; i < r; ++i) {
    ranks_for_file[i] = q + 1;
  } 
  int ranks_so_far = 0;
  int result = -1;
  for ( int i = 0; i < nfiles; ++i) {
    ranks_so_far += ranks_for_file[i];
    if (my_rank < ranks_so_far) {
      result = i;
      break;
    }
  }
  return result;
}

art_params
art_parameters_for_rank(std::string const& events_file)
{
  std::vector<file_record> const files = read_file_records(events_file);
  int my_rank;
  int nranks;
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &nranks);
  int const index = calculate_index(files.size(), my_rank, nranks);
  MPI_Comm local;
  MPI_Comm_split(MPI_COMM_WORLD, index, 0, &local);
  int local_rank;
  int local_size;
  MPI_Comm_rank(local, &local_rank);
  MPI_Comm_size(local, &local_size);
  int const events_in_file = files[index].nevents;
  auto [nskip, nevents] = skip_for_rank(local_rank, local_size, events_in_file);
  return {files[index].input_file, nskip, nevents};
}

// This function executes art
//
void
load_data(std::string const& fcl_file,
          std::string const& input_file,
          int nskip,
          int nevents)
{
  std::string const cmd = fmt::format(
    "art -c {} --nskip {} -n {} -s {}", fcl_file, nskip, nevents, input_file);
  std::system(cmd.c_str());
 // int my_rank;
 //  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
 // std::cout << my_rank << ": " << cmd << std::endl;
}

int
main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr
      << "Please specify fcl file and events file names\n";
    return 1;
  }
  std::vector<std::string> args(argv + 1, argv + argc);

  // initialize MPI after the configuration is all done and checked
  int rc = MPI_Init(&argc, &argv);
  if (rc != MPI_SUCCESS) {
    std::cerr << "MPI initialization failed\n";
    return 1;
  }

  auto [input_file, nskip, events] = art_parameters_for_rank(args[1]);
  load_data(args[0], input_file, nskip, events);
  MPI_Finalize();
}
