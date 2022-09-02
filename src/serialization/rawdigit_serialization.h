
#if !defined(rawdigit_serialization_h)
#define rawdigit_serialization_h

#include <iostream>

#include <lardataobj/RawData/RawDigit.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

//comparison function for RawDigit
//
namespace raw {

  inline bool
    operator==(const RawDigit& a, const RawDigit& b) {
      if (a.Channel() != b.Channel()) return false;
      if (a.Samples() != b.Samples()) return false;
      if (a.GetPedestal() != b.GetPedestal()) return false;
      if (a.GetSigma() != b.GetSigma()) return false;
      if (a.Compression() != b.Compression()) return false;
      if (a.ADCs() != b.ADCs()) return false;

      return true;
    }
}

namespace boost {
  namespace serialization {

    template <class Archive>
      void
      save(Archive& ar, const raw::RawDigit& digi, const unsigned int)
      {
        auto c = digi.Channel();
        ar << c;
        auto s = digi.Samples();
        ar << s;
        auto p = digi.GetPedestal();
        ar << p;
        auto sig = digi.GetSigma();
        ar << sig;
        auto comp= digi.Compression();
        ar << comp;
        ar << digi.ADCs();
      }

    template <class Archive>
      void
      load(Archive& ar, raw::RawDigit & digi, const unsigned int)
      {

        raw::RawDigit::ADCvector_t ADC;
        raw::ChannelID_t channel;
        ULong64_t        samples;
        float            pedestal;
        float            sigma;
        raw::Compress_t  compression;
        ar >> channel;
        ar >> samples;
        ar >> pedestal;
        ar >> sigma;
        ar >> compression;
        ar >> ADC;
        raw::RawDigit rd(channel, samples, std::move(ADC), compression);
        rd.SetPedestal(pedestal, sigma);
        digi = rd;
      }
  }
}

BOOST_SERIALIZATION_SPLIT_FREE(raw::RawDigit)
#endif
