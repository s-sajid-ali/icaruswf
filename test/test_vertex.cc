#include "catch.hpp"

#include "../src/serialization/vertex_serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <utility>

TEST_CASE("writing a vertex works")
{
  double chi2;
  int    dof;
  int    id;
  std::array<
    ROOT::Math::PositionVector3D<
    ROOT::Math::Cartesian3D<Coord_t>,
    ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
    3> pos = {1.0, 2.0, 3.0};
  std::array<
    Double32_t,
    6> matarray = {4.0, 5.0, 6.0, 7.0, 8.0, 9.0};


  recob::Vertex v1(static_cast<const recob::tracking::Point_t&>( recob::tracking::toPoint(pos)),
      recob::tracking::SMatrixSym33(matarray.begin(), matarray.end()),
      chi2, dof, id);

  {
    std::ofstream ofs("vertexout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << v1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("vertexout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Vertex v2;
  ia >> v2;
  CHECK(v1 == v2);

  std::for_each(std::begin(pos), std::end(pos), [](auto& x) { x += 10; });
  recob::Vertex v3(static_cast<const recob::tracking::Point_t&>( recob::tracking::toPoint(pos)),
      recob::tracking::SMatrixSym33(matarray.begin(), matarray.end()),
      chi2, dof, id);
  CHECK_THROWS(v1 == v3);

}
