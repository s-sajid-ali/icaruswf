#if !defined(eid_serialization_h)
#define eid_serialization_h

#include <iostream>

#include "src/modules/EID.h"
#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
  namespace serialization {

    template <class Archive>
      void
      save(Archive& ar, const EID& e, const unsigned int)
      {
        ar << e.run;
        ar << e.subrun;
        ar << e.event;
      }

    template <class Archive>
      void
      load(Archive& ar, EID& e, const unsigned int version)
      {
        ar >> e.run;
        ar >> e.subrun;
        ar >> e.event;
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(EID)
#endif
