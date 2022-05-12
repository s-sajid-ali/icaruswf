#if !defined(trackhitmeta_serialization_h)
#define trackhitmeta_serialization_h

#include <iostream>


#include <lardataobj/RecoBase/TrackHitMeta.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

#include "utils.h"

inline bool
operator==(const recob::TrackHitMeta& t1, const recob::TrackHitMeta& t2) {
  if (t1.Index() != t2.Index()) {
    throw std::runtime_error("Hit indices along the track trajectory are not same");
    return false;}
  if (t1.Dx() != t2.Dx()) {
    throw std::runtime_error("Lengths of the track segments associated with the 2D hit are not same");
    return false;}

  return true;
}


namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::TrackHitMeta& t, const unsigned int)
      {
        unsigned int fidx = t.Index();
        double fdx = t.Dx();

        ar << fidx << fdx;
      }
    template <class Archive>
      void
      load(Archive& ar, recob::TrackHitMeta& t, const unsigned int)
      {
        unsigned int fidx;
        double fdx;

        ar >> fidx >> fdx;
        t = recob::TrackHitMeta(fidx, fdx);
      }

  }
}

BOOST_SERIALIZATION_SPLIT_FREE(recob::TrackHitMeta)
#endif
