#include <hepnos.hpp>
#include "src/modules/EID.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  std::vector<std::string> args(argv+1, argv+argc);
  if (args.size() < 4) {
    std::cerr << "Usage: cheesyQueue_maker <protocol name> <connection file> [queuename ...]";
    return 1; 
  }
  hepnos::DataStore datastore = hepnos::DataStore::connect(args[0], args[1]);
  auto make_queue = [&](std::string const& name) {datastore.createQueue<EID>(name);};
  std::for_each(args.begin()+2, args.end(), make_queue); 
}
