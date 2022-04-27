#include "catch.hpp"

#include "../src/serialization/opflash_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("writing an opflash works")
{
  double              time = 1.1;
  double              timewidth = 2.2;
  double              abstime = 3.3;
  unsigned int        frame = 4.4;
  std::vector<double> peperod = {1.1, 2.2, 3.3};
  std::vector<double> wirecenters = {4.4, 5.5, 6.6};
  std::vector<double> wirewidths = {7.7, 8.8, 9.9};
  double              xcenter = 5.5;
  double              xwidth = 6.6;
  double              ycenter = 7.7;
  double              ywidth = 8.8;
  double              zcenter = 9.9;
  double              zwidth = 1.1;
  double              ftt = 2.2;
  bool                ibf = true;
  int                 obt = 1;

  recob::OpFlash o1(time, timewidth, abstime, frame, peperod, ibf, obt, ftt,
      xcenter, xwidth, ycenter, ywidth, zcenter, zwidth, wirecenters, wirewidths);

  {
    std::ofstream ofs("opflashout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << o1;
    CHECK(ofs.good());
    ofs.close();
  }


  std::ifstream ifs("opflashout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::OpFlash o2;
  ia >> o2;
  CHECK(o1 == o2);

  recob::OpFlash o3(time+1, timewidth, abstime, frame, peperod, ibf, obt, ftt,
      xcenter, xwidth, ycenter, ywidth, zcenter, zwidth, wirecenters, wirewidths);

  CHECK_THROWS(o1==o3);

}
