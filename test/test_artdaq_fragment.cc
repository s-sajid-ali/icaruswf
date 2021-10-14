#include "catch.hpp"

#include "../artdaq_fragment_serialization.h"
#include "artdaq-core/Data/Fragment.hh"
#include "artdaq-core/Core/QuickVec.hh"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

using datavec = artdaq::QuickVec<unsigned long long>;

TEST_CASE("QuickVec comparison works")
{
  std::vector<unsigned long long> v1{4, 5, 6, 7};
  std::vector<unsigned long long> v2{4, 5, 6, 7, 8};
  datavec d1{v1};
  datavec d2{v2};
  datavec d3{v1};
  CHECK(!(d1 == d2));
  CHECK(d1 == d3);
}

TEST_CASE("Writing a QuickVec works")
{
  std::vector<unsigned long long> v{4, 5, 6};
  datavec d{v};
  {
    std::ofstream ofs("qvecout");
    boost::archive::binary_oarchive oa(ofs);
    oa << d;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("qvecout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  std::vector<unsigned long long> u;
  datavec s{u};
  ia >> s;
  CHECK(d == s);
}


TEST_CASE("writing an artdaq Fragment works")
{
  std::vector<unsigned long long> vec{4, 5, 6,7};
  datavec d{vec};
  artdaq::Fragment frag;
  frag.swap(d);
  {
    std::ofstream ofs("fragout");
    boost::archive::binary_oarchive oa(ofs);
    oa << frag;
    CHECK(ofs.good());
    ofs.close();
  }
  std::ifstream ifs("fragout");
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  artdaq::Fragment f;
  CHECK(!(f == frag));
  ia >> f;
  CHECK(f == frag);
}
