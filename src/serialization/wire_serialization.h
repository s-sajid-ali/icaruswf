#include <iostream>

#include <lardataobj/RecoBase/Wire.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace recob {

  inline bool
    operator==(const recob::Wire& aw, const recob::Wire& bw) {
      if(aw.Channel() != bw.Channel()) return false;
      if(aw.View() != bw.View()) return false;
      auto const a = aw.SignalROI();
      auto const b = bw.SignalROI();
      if(a.size() != b.size()) return false;
      for(auto i=0;i<a.size();++i)
        if (a[i] != b[i]) return false;
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const lar::sparse_vector<float>& sv, const unsigned int) {
        auto sz = sv.size();
        ar << sz;
        for (auto i=0;i<sz; ++i)
          ar << sv[i];
      }

    template<class Archive>
      void
      load(Archive&ar, lar::sparse_vector<float>& sv, const unsigned int) {
        size_t sz;
        ar >> sz;
        std::vector<float> tmp;
        tmp.resize(sz);
        for (auto i = 0 ; i<sz; ++i) {
          ar >> tmp[i];
        }
        sv = lar::sparse_vector<float>(tmp, 0);

      }

    template <class Archive>
      void
      save(Archive& ar, const recob::Wire& wire, const unsigned int) {
        auto c = wire.Channel();
        ar << c;
        auto v =  wire.View();
        ar <<  v;
        auto roi = wire.SignalROI();
        ar << roi;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Wire& wire, const unsigned int) {
        raw::ChannelID_t channel;
        geo::View_t view;
        lar::sparse_vector<float> rois;
        ar >> channel;
        ar >> view;
        ar >> rois;
        recob::Wire w(rois, channel, view);
        wire = w;
      }

  }
}
BOOST_SERIALIZATION_SPLIT_FREE(lar::sparse_vector<float>);
BOOST_SERIALIZATION_SPLIT_FREE(recob::Wire);
