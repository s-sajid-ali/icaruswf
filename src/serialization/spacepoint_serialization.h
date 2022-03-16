#if !defined(spacepoint_serialization_h)
#define spacepoint_serialization_h
#include <iostream>

#include <lardataobj/RecoBase/SpacePoint.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

#include "utils.h"

namespace recob {

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
#endif
