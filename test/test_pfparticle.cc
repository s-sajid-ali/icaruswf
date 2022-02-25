#include "catch.hpp"
#include "../pfparticle_serialization.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("PFParticle comparison works")
{
  std::vector<size_t> daughters{2, 4, 6, 8, 9};
  int pdg = 56;
  size_t self = 9;
  size_t parent = 81;
  recob::PFParticle pf1(pdg, self, parent, daughters);
  recob::PFParticle pf2(0, self, parent, daughters);
  CHECK(!(pf1 == pf2));
}

TEST_CASE("writing a PFParticle works")
{
  std::vector<size_t> daughters{2, 4, 6, 8, 9};
  int pdg = 56;
  size_t self = 9;
  size_t parent = 81;
  recob::PFParticle pf1(pdg, self, parent, daughters);
  {
    std::ofstream ofs("pfparticleout");
    boost::archive::binary_oarchive oa(ofs);
    oa << pf1;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("pfparticleout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::PFParticle pf2; 
  CHECK(!(pf1 == pf2));
  ia >> pf2;
  CHECK( pf1 == pf2 );
}
