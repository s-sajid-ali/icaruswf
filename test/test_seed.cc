#include "catch.hpp"

#include "../src/serialization/seed_serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

TEST_CASE("writing a seed works")
{

  std::array<double, 3> Pt     = {1.1,2.1,3.1};
  std::array<double, 3> Dir    = {1.2,2.2,3.2};
  std::array<double, 3> PtErr  = {1.3,2.3,3.3};
  std::array<double, 3> DirErr = {1.4,2.4,3.4};

  recob::Seed s1(Pt.data(), Dir.data(), PtErr.data(), DirErr.data());

  {
    std::ofstream ofs("seedout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << s1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("seedout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Seed s2;
  ia >> s2;

  CHECK(s1 == s2);
}
