#include <iostream>
#include <algorithm>

#include <lardataobj/RecoBase/Vertex.h>
#include <lardataobj/RecoBase/TrackingTypes.h>

#include <boost/serialization/array.hpp>

#include "utils.h"

namespace recob {
  inline bool
    operator==(const recob::Vertex& v1, const recob::Vertex& v2) {
      if (v1.ID() != v2.ID()) {
        throw std::runtime_error("id's are not same");
        return false;}
      if (v1.status() != v2.status()) {
        throw std::runtime_error("statuses are not same");
        return false;}
      if (v1.chi2() != v2.chi2()) {
        throw std::runtime_error("chi2's are not same");
        return false;}
      if (v1.ndof() != v2.ndof()) {
        throw std::runtime_error("ndof's are not same");
        return false;}
      std::array<
        ROOT::Math::PositionVector3D<
        ROOT::Math::Cartesian3D<Coord_t>,
        ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
        3> pos1, pos2;
      v1.position().GetCoordinates(pos1.begin(), pos1.end());
      v2.position().GetCoordinates(pos2.begin(), pos2.end());
      if (compare(pos1, pos2 , 3) == false ) {
        throw std::runtime_error("Position point components are different!");
        return false;}
      std::array<
        Double32_t,
        6> matarray1, matarray2;
      std::copy(v1.covariance().begin(), v1.covariance().end(), matarray1.data() );
      std::copy(v2.covariance().begin(), v2.covariance().end(), matarray2.data() );
      if (compare(matarray1, matarray2 , 6) == false ) {
        throw std::runtime_error("Matrix array components are different!");
        return false;}
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Vertex& v, const unsigned int)
      {

        double chi2 = v.chi2();
        int    dof = v.ndof();
        int    id = v.ID();
        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> pos;
        v.position().GetCoordinates(pos.begin(), pos.end());

        std::array<
          Double32_t,
          6> matarray;
        std::copy(v.covariance().begin(), v.covariance().end(), matarray.data() );

        // status is not serialized as the constructor does not require it!

        ar << chi2;
        ar << dof;
        ar << id;
        ar << pos;
        ar << matarray;
      }


    template <class Archive>
      void
      load(Archive& ar, recob::Vertex& v, const unsigned int)
      {
        double chi2;
        int    dof;
        int    id;

        std::array<
          ROOT::Math::PositionVector3D<
          ROOT::Math::Cartesian3D<Coord_t>,
          ROOT::Math::GlobalCoordinateSystemTag>::Scalar,
          3> pos;
        std::array<
          Double32_t,
          6> matarray;

        ar >> chi2;
        ar >> dof;
        ar >> id;
        ar >> pos;
        ar >> matarray;

        v = recob::Vertex(static_cast<const recob::tracking::Point_t&>( recob::tracking::toPoint(pos)),
            recob::tracking::SMatrixSym33(matarray.begin(), matarray.end()),
            chi2, dof, id);
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::Vertex)
