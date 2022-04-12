#include "catch.hpp"

#include "../src/serialization/ophit_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("writing an ophit works")
{
  int                   channel = 1;
  unsigned short        frame = 2;
  double                pt = 3.3;
  double                ptabs = 4.4;
  double                width = 5.5;
  double                area = 6.6;
  double                amp = 7.7;
  double                pe = 8.8;
  double                ftt = 9.9;

  recob::OpHit o1(channel, pt, ptabs, frame, width, area, amp, pe, ftt);

  {
    std::ofstream ofs("ophitout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << o1;
    CHECK(ofs.good());
    ofs.close();
  }


  std::ifstream ifs("ophitout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::OpHit o2;
  ia >> o2;
  CHECK(o1 == o2);

  recob::OpHit o3(channel+1, pt, ptabs, frame, width, area, amp, pe, ftt);
  CHECK_THROWS(o1==o3);

}
