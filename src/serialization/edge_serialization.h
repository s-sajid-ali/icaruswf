
#if !defined(edge_serialization_h)
#define edge_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/Edge.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace recob {

  inline bool
    operator==(const recob::Edge& ae, const recob::Edge& be) {
      if(ae.Length() != be.Length()) return false;
      if(ae.FirstPointID() != be.FirstPointID()) return false;
      if(ae.SecondPointID() != be.SecondPointID()) return false;
      if(ae.ID() != be.ID()) return false;
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Edge& e, const unsigned int)
      {
        double a = e.Length();
        int b = e.FirstPointID();
        int c = e.SecondPointID();
        unsigned int d = e.ID();
        ar << a << b << c << d;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Edge& e, const unsigned int)
      {
        double a;
        int b, c;
        unsigned int d;
        ar >> a >> b >> c >> d;
        recob::Edge edge(a, b, c, d);
        e = edge;
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::Edge)
#endif
