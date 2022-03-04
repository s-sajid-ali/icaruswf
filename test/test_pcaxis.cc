#include "catch.hpp"

#include "../src/serialization/pcaxis_serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <utility>

TEST_CASE("writing a pcaxis works")
{

  bool   svdstatus = true;
  int    nhits     = 10;
  double hitdoca   = 12;
  size_t fid       = 13;
  std::array<double, 3> eigvals;
  std::array<double, 3> avpos;
  recob::PCAxis::EigenVectors eigvecs;

  std::iota(eigvals.begin(), eigvals.end(), 0);
  std::iota(avpos.begin(), avpos.end(), 1);

  eigvecs.resize(3);
  for (int i=0; i<3; i++){
    eigvecs[i].resize(3);
    std::iota(eigvecs[i].begin(), eigvecs[i].end(), i);
  }

  recob::PCAxis p1(svdstatus, nhits, static_cast<const double*>(eigvals.data()),
      static_cast<const recob::PCAxis::EigenVectors&>(eigvecs),
      static_cast<const double*>(avpos.data()), static_cast<const double>(hitdoca), fid);

  {
    std::ofstream ofs("pcaxisout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << p1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("pcaxisout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::PCAxis p2;
  ia >> p2;
  CHECK(p1 == p2);
}
