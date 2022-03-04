#include "catch.hpp"

#include "../src/serialization/wire_serialization.h"
#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("Wire comparison works")
{
  std::vector<float> tmp{2.2, 4.3, 6.5, 8.7, 9.0};
  raw::ChannelID_t channel = 56;
  geo::View_t view = geo::k3D;
  lar::sparse_vector<float> sv(tmp, 0);
  recob::Wire wire(sv, channel, view);
  std::vector<float> tmp1{2.2, 4.3, 6.6, 8.7, 9.0};
  lar::sparse_vector<float> sv1(tmp1, 0);
  recob::Wire wire1(sv1, channel, view);
  CHECK(!(wire1 == wire)); // to check wires constructed from unequal sparse_vectors
}

TEST_CASE("writing a wire works")
{
  std::vector<float> tmp{2.2, 4.3, 6.5, 8.7, 9.0};
  raw::ChannelID_t channel = 56;
  geo::View_t view = geo::k3D;
  lar::sparse_vector<float> sv(tmp, 0);
  recob::Wire wire(sv, channel, view);
  {
    std::ofstream ofs("wireout");
    boost::archive::binary_oarchive oa(ofs);
    oa << wire;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("wireout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Wire w;
  CHECK(!(w == wire));
  ia >> w;
  CHECK( wire == w );
}
