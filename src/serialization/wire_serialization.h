#if !defined(wire_serialization_h)
#define wire_serialization_h

#include <lardataobj/RecoBase/Wire.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

using svf = lar::sparse_vector<float>;
using svfrange = svf::datarange_t;

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
      save(Archive& ar, const svfrange& dr, const unsigned int) {
        ar << dr.begin_index();
        // there is no need to write end index, it is redundant
        ar << dr.data();
      }

    template<class Archive>
      void
      load(Archive&ar, svfrange& dr, const unsigned int) {
        size_t offset{};
        ar >> offset;
        std::vector<float> tmp;
        ar >> tmp; 
        dr = svfrange(offset, std::move(tmp));
      }

    template <class Archive>
      void
      save(Archive& ar, const lar::sparse_vector<float>& sv, const unsigned int) {
        auto nominal_size = sv.size(); 
        ar << nominal_size;
        ar << sv.get_ranges();
      }

    template<class Archive>
      void
      load(Archive&ar, lar::sparse_vector<float>& sv, const unsigned int) {
        size_t nominal_size;
        ar >> nominal_size;
        if(sv.size() > nominal_size) sv.clear();
        sv.resize(nominal_size);
        std::vector<svfrange> tmp;
        ar >> tmp; 
        using svfranges = std::vector<svfrange>;
        // we are casting away the constness because sparse vector 
        // provides no efficient way to assemble its data
        svfranges & dest = const_cast<svfranges &>(sv.get_ranges());
        dest = std::move(tmp);
      }

    template <class Archive>
      void
      save(Archive& ar, const recob::Wire& wire, const unsigned int) {
        auto c = wire.Channel();
        ar << c;
        auto v =  wire.View();
        ar <<  v;
        auto& roi = wire.SignalROI();
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
        recob::Wire w(std::move(rois), std::move(channel), std::move(view));
        wire = std::move(w);
      }

  }
}
BOOST_SERIALIZATION_SPLIT_FREE(lar::sparse_vector<float>::datarange_t);
BOOST_SERIALIZATION_SPLIT_FREE(lar::sparse_vector<float>);
BOOST_SERIALIZATION_SPLIT_FREE(recob::Wire);
#endif
