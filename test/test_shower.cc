#include "catch.hpp"

#include "../src/serialization/shower_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>

TEST_CASE("writing an shower works")
{

  std::vector<double> ftotenergy {1.1,2.2,3.3};
  std::vector<double> fsigtotenergy {4.4,5.5,6.6};
  std::vector<double> fdedx {1.11,2.22,3.33};
  std::vector<double> fsigedx {4.44,5.55,6.66};
  int fid = 1;
  int fbp = 2;
  double flen = 3.3;
  double foa = 4.4 ;
  const TVector3 _fdcs(1.1,2.2,3.3);
  const TVector3 _fsigdcs(4.4,5.5,6.6);
  const TVector3 _fsxyz(7.7,8.8,9.9);
  const TVector3 _fsigxyz(1.11,2.22,3.33);

  recob::Shower s1(_fdcs, _fsigdcs, _fsxyz, _fsigxyz,
      ftotenergy, fsigtotenergy, fdedx, fsigedx,
      fbp, fid, flen, foa);

  {
    std::ofstream ofs("showerout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << s1;
    CHECK(ofs.good());
    ofs.close();
  }


  std::ifstream ifs("showerout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Shower s2;
  ia >> s2;
  CHECK(s1 == s2);

  recob::Shower s3(_fdcs, _fsigdcs, _fsxyz, _fsigxyz,
      ftotenergy, fsigtotenergy, fdedx, fsigedx,
      fbp, fid, flen, foa-1);


  CHECK_THROWS(s1==s3);

}
