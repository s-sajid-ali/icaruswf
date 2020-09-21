#include "lardataobj/RecoBase/Wire.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/Utilities/sparse_vector.h"

#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace boost {
  namespace serialization {
    template <class Archive>
    void
    save(Archive& ar, const recob::Wire& wire, const unsigned int) {
      ar << wire.Channel();
      ar << wire.View();
      ar <<  wire.SignalROI();
    }

    template <class Archive>
    void
    load(Archive& ar, recob::Wire& wire, const unsigned int) {
      raw::ChannelID_t channel;
      geo::View_t view;
      RegionsOfInterest_t rois;

      recob::Wire w();
      wire = w;
    }
  
  }
}
