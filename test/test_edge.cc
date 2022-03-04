#include "catch.hpp"

#include "../src/serialization/edge_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("Edge comparison works")
{
  recob::Edge e1(20., 3, 4, 5);
  recob::Edge e2(20., 3, 4, 5);
  recob::Edge e3(45., 3, 4, 5);
  CHECK(e1==e2);
  CHECK(!(e1==e3));
}

TEST_CASE("writing an edge works")
{
  recob::Edge edge(20., 3, 4, 5);
  {
    std::ofstream ofs("edgeout");
    boost::archive::binary_oarchive oa(ofs);
    oa << edge;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("edgeout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Edge e;
  CHECK(!(e == edge));
  ia >> e;
  CHECK( e == edge );
}
