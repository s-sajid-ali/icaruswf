#ifndef HEPNOSDATASTORESERVICE_H
#define HEPNOSDATASTORESERVICE_H

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Services/Registry/ServiceTable.h"
#include "fhiclcpp/types/Atom.h"

#include "hepnos.hpp"



namespace icaruswf {
class HepnosDataStore {
public:
  struct Config {
    fhicl::Atom<std::string> protocol{fhicl::Name("protocol"), "ofi+tcp"};
    fhicl::Atom<std::string> connection_file{fhicl::Name("connection_file"), "connection.json"};
  };
  using Parameters = art::ServiceTable<Config>;
  explicit HepnosDataStore(Parameters const & ps);
  hepnos::DataStore& getStore();

private:
  hepnos::DataStore dataStore_;
};
}

DECLARE_ART_SERVICE(icaruswf::HepnosDataStore, SHARED)

#endif
