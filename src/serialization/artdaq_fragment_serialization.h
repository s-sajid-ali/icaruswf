#include <iostream>

#include <artdaq-core/Data/Fragment.hh>
#include <artdaq-core/Core/QuickVec.hh>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

using datavec = artdaq::QuickVec<unsigned long long>;

//comparison functions for QuickVec and Fragment
namespace artdaq {
  inline bool
    operator==(datavec const& a, datavec const& b) {
      if (a.size() != b.size()) return false;
      if (a.capacity() != b.capacity()) return false;

      for (int i = 0 ; i!= a.size(); ++i)
        if (a[i] != b[i]) return false;

      return true;
    }

  artdaq::Fragment make_fragment(size_t a, std::vector<unsigned long long>v) {
    datavec vals{v};
    artdaq::Fragment f;
    f.swap(vals);
    return f;
  }

  inline bool
    operator==(Fragment const& a, Fragment const& b)
    {
      auto da = reinterpret_cast<datavec const*>(&a);
      auto db = reinterpret_cast<datavec const*>(&b);
      return *da == *db;
    }
}


//serialization
namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const datavec& d, const unsigned int)
      {
        //datavec& d = const_cast<datavec &>(cd);
        unsigned s = d.size();
        ar << s;
        unsigned c = d.capacity();
        ar << c;
        for (auto it = d.begin(); it != d.end(); ++it)
          ar << *it;
      }

    template <class Archive>
      void
      load(Archive& ar, datavec& d, const unsigned int)
      {
        unsigned s;
        unsigned c;
        ar >> s;
        ar >> c;
        std::vector<unsigned long long> data;
        data.resize(c);
        for ( int i = 0; i != s; ++i)
          ar >> data[i];
        d = datavec(data);
      }

    //save and load for artdaq::Fragment will break if implementation of Fragment grows a base class or any data members before the quickvec
    template <class Archive>
      void
      save(Archive& ar, const artdaq::Fragment& f, const unsigned int)
      {
        //
        auto dv = reinterpret_cast<datavec const*>(&f);
        ar << *dv;
      }

    template <class Archive>
      void
      load(Archive& ar, artdaq::Fragment& f, const unsigned int)
      {
        datavec* d = reinterpret_cast<datavec*>(&f);
        ar >> *d;
      }
  }
}
  BOOST_SERIALIZATION_SPLIT_FREE(datavec)
BOOST_SERIALIZATION_SPLIT_FREE(artdaq::Fragment)
