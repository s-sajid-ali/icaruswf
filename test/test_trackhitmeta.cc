#include "catch.hpp"

#include "../src/serialization/trackhitmeta_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("writing an trackhitmeta works")
{
  unsigned int fidx = 1;
  double fdx = 1.1;

  recob::TrackHitMeta t1(fidx, fdx);

  {
    std::ofstream ofs("trackhitmetaout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << t1;
    CHECK(ofs.good());
    ofs.close();
  }


  std::ifstream ifs("trackhitmetaout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::TrackHitMeta t2;
  ia >> t2;
  CHECK(t1 == t2);

  recob::TrackHitMeta t3(fidx+1, fdx);
  CHECK_THROWS(t1==t3);

}
