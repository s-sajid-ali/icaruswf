#if !defined(opdetwaveform_serialization_h)
#define opdetwaveform_serialization_h

#include <iostream>

#include <lardataobj/RawData/OpDetWaveform.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

#include "utils.h"

inline bool
operator==(const raw::OpDetWaveform& o1, const raw::OpDetWaveform& o2) {
  if (o1.ChannelNumber() != o2.ChannelNumber()) {
    throw std::runtime_error("channel number values are not same");
    return false;}
  if (o1.TimeStamp() != o2.TimeStamp()) {
    throw std::runtime_error("timestamp values are not same");
    return false;}
  const std::vector<raw::ADC_Count_t>& v1 = static_cast<const std::vector<raw::ADC_Count_t>&>(o1);
  const std::vector<raw::ADC_Count_t>& v2 = static_cast<const std::vector<raw::ADC_Count_t>&>(o2);

  if (v1.size() != v2.size()) {
    throw std::runtime_error("vector sizes are not same");
    return false;}
  if (compare(v1, v2, o1.size()) == false) {
    throw std::runtime_error("vector components are different!");
    return false;}
  return true;
}


namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const raw::OpDetWaveform& o, const unsigned int)
      {
        raw::Channel_t c = o.ChannelNumber();
        raw::TimeStamp_t t = o.TimeStamp();
        const std::vector<raw::ADC_Count_t>& v = static_cast<const std::vector<raw::ADC_Count_t>&>(o);
        size_t len = v.size();
        ar << c << t << len << v;
      }
    template <class Archive>
      void
      load(Archive& ar, raw::OpDetWaveform& o, const unsigned int)
      {
        raw::Channel_t c;
        raw::TimeStamp_t t;
        size_t len;

        ar >> c >> t >> len;

        o = raw::OpDetWaveform(c, t, len);
        std::vector<raw::ADC_Count_t>& v = static_cast<std::vector<raw::ADC_Count_t>&>(o);
        ar >> v;
      }
  }
}

BOOST_SERIALIZATION_SPLIT_FREE(raw::OpDetWaveform)
#endif
