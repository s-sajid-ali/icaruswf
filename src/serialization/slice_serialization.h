#include <iostream>

#include <lardataobj/RecoBase/Slice.h>
#include <lardataobj/RecoBase/TrackingTypes.h>

#include <boost/serialization/array.hpp>

#include "utils.h"

namespace recob {
  inline bool
    operator==(const recob::Slice& s1, const recob::Slice& s2) {
      if (s1.ID() != s2.ID()) {
        throw std::runtime_error("id's are not same");
        return false;}
      if (s1.AspectRatio() != s2.AspectRatio()) {
        throw std::runtime_error("aspect ratios are not same");
        return false;}
      if (s1.Charge() != s2.Charge()) {
        throw std::runtime_error("charges are not same");
        return false;}
      std::array<
        ROOT::Math::PositionVector3D<
        ROOT::Math::Cartesian3D<Coord_t>,
        ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
        3> fctr1,fctr2;
      s1.Center().GetCoordinates(fctr1.begin(), fctr1.end());
      s2.Center().GetCoordinates(fctr2.begin(), fctr2.end());
      if (compare(fctr1, fctr2 , 3) == false ) {
        throw std::runtime_error("Center point components are different!");
        return false;}
      std::array<
        ROOT::Math::PositionVector3D<
        ROOT::Math::Cartesian3D<Coord_t>,
        ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
        3> f1e0pos,f2e0pos;
      s1.End0Pos().GetCoordinates(f1e0pos.begin(), f1e0pos.end());
      s2.End0Pos().GetCoordinates(f2e0pos.begin(), f2e0pos.end());
      if (compare(f1e0pos, f2e0pos, 3) == false ) {
        throw std::runtime_error("End 0 position components are different!");
        return false;}
      std::array<
        ROOT::Math::PositionVector3D<
        ROOT::Math::Cartesian3D<Coord_t>,
        ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
        3> f1e1pos, f2e1pos;
      s1.End1Pos().GetCoordinates(f1e1pos.begin(), f1e1pos.end());
      s2.End1Pos().GetCoordinates(f2e1pos.begin(), f2e1pos.end());
      if (compare(f1e1pos, f2e1pos, 3) == false ) {
        throw std::runtime_error("End 1 position components are different!");
        return false;}
      std::array<
        ROOT::Math::DisplacementVector3D<
        ROOT::Math::Cartesian3D<Coord_t>,
        ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
        3> f1dir,f2dir;
      s1.Direction().GetCoordinates(f1dir.begin(), f1dir.end());
      s2.Direction().GetCoordinates(f2dir.begin(), f2dir.end());
      if (compare(f1dir, f2dir, 3) == false ) {
        throw std::runtime_error("Direction vector components are different!");
        return false;}

      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Slice& s, const unsigned int)
      {
        int     fid = s.ID();
        float   faspectratio = s.AspectRatio();
        float   fcharge = s.Charge();

        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fctr;
        s.Center().GetCoordinates(fctr.begin(), fctr.end());

        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fe0pos;
        s.End0Pos().GetCoordinates(fe0pos.begin(), fe0pos.end());

        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fe1pos;
        s.End1Pos().GetCoordinates(fe1pos.begin(), fe1pos.end());

        std::array<
          ROOT::Math::DisplacementVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fdir;
        s.Direction().GetCoordinates(fdir.begin(), fdir.end());


        ar << fid;
        ar << fctr;
        ar << fdir;
        ar << fe0pos;
        ar << fe1pos;
        ar << faspectratio;
        ar << fcharge;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Slice& s, const unsigned int)
      {
        int      fid;
        float    faspectratio;
        float    fcharge;

        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fctr;
        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fe0pos;
        s.Center().GetCoordinates(fe0pos.begin(), fe0pos.end());
        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fe1pos;
        std::array<
          ROOT::Math::DisplacementVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> fdir;

        ar >> fid;
        ar >> fctr;
        ar >> fdir;
        ar >> fe0pos;
        ar >> fe1pos;
        ar >> faspectratio;
        ar >> fcharge;

        s = recob::Slice (fid,
            recob::tracking::toPoint(fctr),
            recob::tracking::toVector(fdir),
            recob::tracking::toPoint(fe0pos),
            recob::tracking::toPoint(fe1pos),
            faspectratio, fcharge);
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::Slice)

