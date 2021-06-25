#include "lardataobj/RecoBase/SpacePoint.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/Utilities/sparse_vector.h"

#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace recob {

bool compare(Double32_t const* a, Double32_t const* b, int len) {
    for(auto i=0;i<len;++i)
      if (a[i] != b[i]) return false;
    return true;
}

inline bool
  operator==(const recob::SpacePoint sp1, const recob::SpacePoint& sp2) {
    if(sp1.ID() != sp2.ID()) return false;
    if(sp1.Chisq() != sp2.Chisq()) return false;
    if(compare(sp1.XYZ(), sp2.XYZ(), 3) == false) return false;
    if(compare(sp1.ErrXYZ(), sp2.ErrXYZ(), 6) == false) return false;
    return true;
  }
}

namespace boost {
  namespace serialization {
    template <class Archive>
    void
    save(Archive& ar, const recob::SpacePoint& sp, const unsigned int) {
      auto id = sp.ID();
      ar << id;
      auto xyz =  sp.XYZ();
      ar << xyz[0] << xyz[1] << xyz[2];
      auto err = sp.ErrXYZ();
      ar << err[0] << err[1] << err[2] << err[3] << err[4] << err[5];
      auto chisq = sp.Chisq();
      ar << chisq;
    }

    template <class Archive>
    void
    load(Archive& ar, recob::SpacePoint& sp, const unsigned int) {
      int id;
      Double32_t xyz[3];
      Double32_t err[6];
      Double32_t chisq;
      ar >> id;
      ar >> xyz[0] ;
      ar >> xyz[1];
      ar >> xyz[2];
      ar >> err[0];
      ar >> err[1];
      ar >> err[2];
      ar >> err[3];
      ar >> err[4];
      ar >> err[5];
      ar >> chisq;
      recob::SpacePoint p(xyz, err, chisq, id); 
      sp = p;
    }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::SpacePoint);
