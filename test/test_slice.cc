#include "catch.hpp"

#include "../src/serialization/slice_serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <utility>

TEST_CASE("writing a slice works")
{

  int     fid = 10;
  float   faspectratio = 2.9;
  float   fcharge = 3.1;
  std::array<
    ROOT::Math::PositionVector3D<
    ROOT::Math::Cartesian3D<Coord_t>,
    ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
    3> fctr = {1.0, 2.0, 3.0};
  std::array<
    ROOT::Math::PositionVector3D<
    ROOT::Math::Cartesian3D<Coord_t>,
    ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
    3> fe0pos = {4.0, 5.0, 6.0};
  std::array<
    ROOT::Math::PositionVector3D<
    ROOT::Math::Cartesian3D<Coord_t>,
    ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
    3> fe1pos = {7.0, 8.0, 9.0};
  std::array<
    ROOT::Math::DisplacementVector3D<
    ROOT::Math::Cartesian3D<Coord_t>,
    ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
    3> fdir = {0.1, 0.2, 0.3};

  recob::Slice s1(fid,
      recob::tracking::toPoint(fctr),
      recob::tracking::toVector(fdir),
      recob::tracking::toPoint(fe0pos),
      recob::tracking::toPoint(fe1pos),
      faspectratio, fcharge);

  {
    std::ofstream ofs("sliceout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << s1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("sliceout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Slice s2;
  ia >> s2;
  CHECK(s1 == s2);

  std::for_each(std::begin(fctr), std::end(fctr), [](auto& x) { x += 10; });
  recob::Slice s3(fid,
      recob::tracking::toPoint(fctr),
      recob::tracking::toVector(fdir),
      recob::tracking::toPoint(fe0pos),
      recob::tracking::toPoint(fe1pos),
      faspectratio, fcharge);

  CHECK_THROWS(s1==s3);

}
