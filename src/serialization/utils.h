#if !defined(utils_h)
#define utils_h
// Common utilities for seriziation

// ROOT Includes!
#include <Math/GenVector/PositionVector3D.h>
#include <Math/GenVector/DisplacementVector3D.h>
// Recob types
#include <lardataobj/RecoBase/TrackingTypes.h>

using root_pvscalar_type= ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<Coord_t>,
      ROOT::Math::GlobalCoordinateSystemTag>::Scalar;
using root_dvscalar_type= ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<Coord_t>,
      ROOT::Math::GlobalCoordinateSystemTag>::Scalar;


// Compare whether all elements C-style arrays are equal.
// While CATCH2 provides matchers for std::vector, most recob
// classes use C-style arrays so the following is useful
// for comparing them during assertions

template <typename T>
bool compare( const T &a, const T &b, int len) {
  for(auto i=0;i<len;++i)
    if (a[i] != b[i]) return false;
  return true;
}
#endif
