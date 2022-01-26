#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "HepnosDataStore.h"

namespace icaruswf {
  HepnosDataStore::HepnosDataStore(Parameters const & ps)
    : dataStore_{hepnos::DataStore::connect(ps().protocol(), ps().connection_file())}
  {}

  hepnos::DataStore& HepnosDataStore::getStore() {
    return dataStore_;
  }
}

DEFINE_ART_SERVICE(icaruswf::HepnosDataStore)

