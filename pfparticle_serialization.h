#include "lardataobj/RecoBase/PFParticle.h"
#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"
#include "lardataobj/Utilities/sparse_vector.h"

#include <iostream>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

namespace recob {

inline bool
  operator==(const recob::PFParticle& ap, const recob::PFParticle& bp) {
    if(ap.PdgCode() != bp.PdgCode()) return false;
    if(ap.Self() != bp.Self()) return false;
    if(ap.Parent() != bp.Parent()) return false;
    auto const a = ap.Daughters();
    auto const b = bp.Daughters();
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
    save(Archive& ar, const recob::PFParticle& pf, const unsigned int) {
      auto p = pf.PdgCode();
      auto s = pf.Self();
      auto pr = pf.Parent();
      ar << p << s << pr;
      auto const a = pf.Daughters();
      ar << a.size();
      for(auto i=0;i<a.size();++i)
        ar << a[i];
    }

    template <class Archive>
    void
    load(Archive& ar, recob::PFParticle& pf, const unsigned int) {
      int p;
      size_t s;
      size_t pr; 
      ar >> p;
      ar >> s;
      ar >> pr;
      std::vector<size_t> ds;
      size_t sz;
      ar >> sz;
      ds.resize(sz);
      for(auto i=0;i<sz;++i) {
        ar >> ds[i];
      }
      recob::PFParticle pfp(p, s, pr, ds);
      pf = pfp;
    }
  
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::PFParticle);
