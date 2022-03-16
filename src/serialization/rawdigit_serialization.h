
#if !defined(rawdigit_serialization_h)
#define rawdigit_serialization_h

#include <iostream>

#include <lardataobj/RawData/RawDigit.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

//comparison function for ADCvector_t and RawDigit
//
namespace raw {
  using ADCvector_t = std::vector<short>;

  inline bool
    operator==(const ADCvector_t& a, const ADCvector_t& b) {
      if (a.size() != b.size()) return false;
      for (int i = 0 ; i!= a.size(); ++i)
        if (a[i] != b[i]) return false;
      return true;
    }

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
      save(Archive& ar, const raw::ADCvector_t& adcs, const unsigned int)
      {
        size_t sz = adcs.size();
        ar << sz;
        for (auto i=0; i< sz; ++i)
          ar << adcs[i];
      }

    template <class Archive>
      void
      load(Archive& ar, raw::ADCvector_t& adcs, const unsigned int)
      {
        size_t sz;
        ar >> sz;
        raw::ADCvector_t adc_vec;
        adc_vec.reserve(sz);
        for (auto i = 0 ; i<sz; ++i) {
          ar >> adc_vec[i];
          adcs.push_back(adc_vec[i]);
        }
      }


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
        raw::ADCvector_t const adcs = digi.ADCs();
        ar << adcs;
      }

    template <class Archive>
      void
      load(Archive& ar, raw::RawDigit & digi, const unsigned int)
      {

        raw::ADCvector_t ADC;
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
        raw::RawDigit rd(channel, samples, ADC, compression);
        rd.SetPedestal(pedestal, sigma);
        digi = rd;
      }
  }
}

BOOST_SERIALIZATION_SPLIT_FREE(raw::RawDigit)
#endif
