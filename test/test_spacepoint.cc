#include "catch.hpp"
#include "../spacepoint_serialization.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("SpacePoint comparison works")
{
  Double32_t xyz[3] = {2.2, 4.3, 6.5};
  Double32_t err[6] = {0.1, 0.01, 0.021, 0.2, 0.3, 0.45};
  int id = 56;
  Double32_t chisq = 3.2;
  
  Double32_t err1[6] = {0.1, 0.01, 0.021, 0.2, 0.78, 0.45};
  recob::SpacePoint sp1(xyz, err, chisq, id);
  recob::SpacePoint sp2(xyz, err1, chisq, id);
  CHECK(!(sp1 == sp2)); // to check wires constructed from unequal sparse_vectors
}

TEST_CASE("writing a spacepoint works")
{
  Double32_t xyz[3] = {2.2, 4.3, 6.5};
  Double32_t err[6] = {0.1, 0.01, 0.021, 0.2, 0.3, 0.45};
  int id = 56;
  Double32_t chisq = 3.2;
  
  recob::SpacePoint sp(xyz, err, chisq, id);
  {
    std::ofstream ofs("spacepointout");
    boost::archive::binary_oarchive oa(ofs);
    oa << sp;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("spacepointout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::SpacePoint sp1; 
  CHECK(!(sp == sp1));
  ia >> sp1;
  CHECK( sp == sp1 );
}
