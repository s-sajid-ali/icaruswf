#include "catch.hpp"

#include "../src/serialization/opdetwaveform_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>

TEST_CASE("writing an opdetwaveform works")
{

  raw::Channel_t c = 1;
  raw::TimeStamp_t t = 1.1;
  size_t len = 3;

  raw::OpDetWaveform o1(c, t, len);
  {
    std::vector<raw::ADC_Count_t>& v = dynamic_cast<std::vector<raw::ADC_Count_t>&>(o1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
  }

  {
    std::ofstream ofs("opdetwaveformout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << o1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("opdetwaveformout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  raw::OpDetWaveform o2;
  ia >> o2;
  CHECK(o1 == o2);

  raw::OpDetWaveform o3(c, t, len);
  {
    std::vector<raw::ADC_Count_t>& v = dynamic_cast<std::vector<raw::ADC_Count_t>&>(o3);
    v.push_back(2);
    v.push_back(3);
    v.push_back(5);
  }
  CHECK_THROWS(o1 == o3);

}
