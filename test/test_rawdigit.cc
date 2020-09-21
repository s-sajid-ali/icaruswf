#include "../../code/catch.hpp"
#include "../rawdigit_serialization.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("writing a rawdigit works")
{
  std::vector<short> adcs{2, 4, 6, 8, 9};
  unsigned int channel = 56;
  ULong64_t samples = 78;
  float sigma = 2.3;
  float pdestal = 6.5;

  raw::RawDigit digi(channel,samples, adcs, raw::kNone);
  {
    std::ofstream ofs("rawdigiout");
    boost::archive::binary_oarchive oa(ofs);
    oa << digi;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("rawdigiout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  raw::RawDigit d; 
  CHECK(!(d == digi));
  ia >> d;
  CHECK(digi == d);
}
