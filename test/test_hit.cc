#include "catch.hpp"
#include "make_hit.h"
#include "../compare_hit.h"
#include "../hit_serialization.h"

#include "lardataobj/RecoBase/Hit.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("hit comparison works")
{
 
  recob::Hit h1 = make_hit(1);
  geo::WireID wireID(geo::PlaneID(geo::TPCID(geo::CryostatID(11), 14), 13), 12);
  recob::Hit h2 = h1; 
  recob::Hit h3(1,
                2,
                5,
                1.5,
                2.5,
                3.5,
                4.5,
                5.5,
                6.5,
                7.5,
                8.5,
                4,
                5,
                9.5,
               -1,
               geo::k3D,
               geo::kCollection,
               wireID); 
  CHECK(h1 == h2);
  CHECK(!(h1 == h3));
}

TEST_CASE("writing a hit works")
{
  recob::Hit h = make_hit(1);
  // make an archive
  {
    std::ofstream ofs("hitout");
    boost::archive::binary_oarchive oa(ofs);
    oa << h;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("hitout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Hit h2;
  CHECK(h2 == recob::Hit());
  ia >> h2;
  CHECK(h2 == h);
}

TEST_CASE("writing a vector<hit> works")
{
  std::vector<recob::Hit> hits {make_hit(1), make_hit(2)};
  {

    std::ofstream ofs("hitsout");
    boost::archive::binary_oarchive oa(ofs);
    oa << hits; 
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("hitsout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  std::vector<recob::Hit> hits2 {make_hit(1), make_hit(3)};
  CHECK(!(hits == hits2));
  ia >> hits2;
  CHECK(hits == hits2);
}
