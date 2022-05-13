#if !defined(shower_serialization_h)
#define shower_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/Shower.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

#include "TVector3.h"

#include "utils.h"

using tv3_scalar_type=TVector3::Scalar;

inline bool
operator==(const recob::Shower& s1, const recob::Shower& s2) {
  if (s1.ID() != s2.ID()) {
    throw std::runtime_error("ID values are not same");
    return false;}
  if (s1.best_plane() != s2.best_plane()) {
    throw std::runtime_error("best plane values are not same");
    return false;}
  if (s1.Length() != s2.Length()) {
    throw std::runtime_error("Length values are not same");
    return false;}
  if (s1.OpenAngle() != s2.OpenAngle()) {
    throw std::runtime_error("OpenAngle values are not same");
    return false;}

  if (s1.Energy().size() != s2.Energy().size()) {
    throw std::runtime_error("total energy vector sizes are different!");
    return false;}
  if (compare(s1.Energy(), s2.Energy() , s1.Energy().size()) == false ) {
    throw std::runtime_error("total energy vector components are different!");
    return false;}

  if (s1.EnergyErr().size() != s2.EnergyErr().size()) {
    throw std::runtime_error("sig total energy vector sizes are different!");
    return false;}
  if (compare(s1.EnergyErr(), s2.EnergyErr() , s1.EnergyErr().size()) == false ) {
    throw std::runtime_error("sig total energy vector components are different!");
    return false;}

  if (s1.dEdx().size() != s2.dEdx().size()) {
    throw std::runtime_error("dedx vector sizes are different!");
    return false;}
  if (compare(s1.dEdx(), s2.dEdx() , s1.dEdx().size()) == false ) {
    throw std::runtime_error("dedx vector components are different!");
    return false;}

  if (s1.dEdxErr().size() != s2.dEdxErr().size()) {
    throw std::runtime_error("dedx err vector sizes are different!");
    return false;}
  if (compare(s1.dEdxErr(), s2.dEdxErr() , s1.dEdxErr().size()) == false ) {
    throw std::runtime_error("dedx err vector components are different!");
    return false;}

  std::array<tv3_scalar_type, 3> fdcs1{s1.Direction().X(), s1.Direction().Y(), s1.Direction().Z()};
  std::array<tv3_scalar_type, 3> fdcs2{s2.Direction().X(), s2.Direction().Y(), s2.Direction().Z()};
  if (compare(fdcs1, fdcs2 , fdcs1.size()) == false ) {
    throw std::runtime_error("fdcs vector components are different!");
    return false;}

  std::array<tv3_scalar_type, 3> fsigdcs1{s1.DirectionErr().X(), s1.DirectionErr().Y(), s1.DirectionErr().Z()};
  std::array<tv3_scalar_type, 3> fsigdcs2{s2.DirectionErr().X(), s2.DirectionErr().Y(), s2.DirectionErr().Z()};
  if (compare(fsigdcs1, fsigdcs2 , fsigdcs1.size()) == false ) {
    throw std::runtime_error("fsigdcs vector components are different!");
    return false;}

  std::array<tv3_scalar_type, 3> fsxyz1{s1.ShowerStart().X(), s1.ShowerStart().Y(), s1.ShowerStart().Z()};
  std::array<tv3_scalar_type, 3> fsxyz2{s2.ShowerStart().X(), s2.ShowerStart().Y(), s2.ShowerStart().Z()};
  if (compare(fsxyz1, fsxyz2 , fsxyz1.size()) == false ) {
    throw std::runtime_error("fsxyz vector components are different!");
    return false;}

  std::array<tv3_scalar_type, 3> fsigxyz1{s1.ShowerStartErr().X(), s1.ShowerStartErr().Y(), s1.ShowerStartErr().Z()};
  std::array<tv3_scalar_type, 3> fsigxyz2{s2.ShowerStartErr().X(), s2.ShowerStartErr().Y(), s2.ShowerStartErr().Z()};
  if (compare(fsigxyz1, fsigxyz2 , fsigxyz1.size()) == false ) {
    throw std::runtime_error("fsigxyz vector components are different!");
    return false;}


  return true;
}


namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Shower& s, const unsigned int)
      {
        const TVector3& _fdcs = s.Direction();
        std::array<tv3_scalar_type, 3> fdcs{_fdcs.X(), _fdcs.Y(), _fdcs.Z()};

        const TVector3& _fsigdcs = s.DirectionErr();
        std::array<tv3_scalar_type, 3> fsigdcs{_fsigdcs.X(), _fsigdcs.Y(), _fsigdcs.Z()};

        const TVector3& _fsxyz = s.ShowerStart();
        std::array<tv3_scalar_type, 3> fsxyz{_fsxyz.X(), _fsxyz.Y(), _fsxyz.Z()};

        const TVector3& _fsigxyz = s.ShowerStartErr();
        std::array<tv3_scalar_type, 3> fsigxyz{_fsigxyz.X(), _fsigxyz.Y(), _fsigxyz.Z()};

        const std::vector<double>& ftotenergy = s.Energy();
        const std::vector<double>& fsigtotenergy = s.EnergyErr();
        const std::vector<double>& fdedx = s.dEdx();
        const std::vector<double>& fsigedx = s.dEdxErr();

        int fid = s.ID() ;
        int fbp = s.best_plane();
        double flen = s.Length();
        double foa = s.OpenAngle();

        ar << fdcs << fsigdcs << fsxyz << fsigxyz << ftotenergy << fsigtotenergy << fdedx << fsigedx << fid << fbp << flen << foa ;
      }
    template <class Archive>
      void
      load(Archive& ar, recob::Shower& s, const unsigned int)
      {
        std::array<tv3_scalar_type, 3> fdcs;
        std::array<tv3_scalar_type, 3> fsigdcs;
        std::array<tv3_scalar_type, 3> fsxyz;
        std::array<tv3_scalar_type, 3> fsigxyz;

        std::vector<double> ftotenergy;
        std::vector<double> fsigtotenergy;
        std::vector<double> fdedx;
        std::vector<double> fsigedx;

        int fid;
        int fbp;
        double flen;
        double foa;

        ar >> fdcs >> fsigdcs >> fsxyz >> fsigxyz >> ftotenergy >> fsigtotenergy >> fdedx >> fsigedx >> fid >> fbp >> flen >> foa ;

        const TVector3 _fdcs(fdcs[0], fdcs[1], fdcs[2]);
        const TVector3 _fsigdcs(fsigdcs[0], fsigdcs[1], fsigdcs[2]);
        const TVector3 _fsxyz(fsxyz[0], fsxyz[1], fsxyz[2]);
        const TVector3 _fsigxyz(fsigxyz[0], fsigxyz[1], fsigxyz[2]);

        s =  recob::Shower(_fdcs, _fsigdcs, _fsxyz, _fsigxyz,
            ftotenergy, fsigtotenergy, fdedx, fsigedx,
            fbp, fid, flen, foa);
      }

  }
}

BOOST_SERIALIZATION_SPLIT_FREE(recob::Shower)
#endif
