#if !defined(seed_serialization_h)
#define seed_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/Seed.h>

#include <boost/serialization/array.hpp>

#include "utils.h"

namespace recob {
  inline bool
    operator==(const recob::Seed& s1, const recob::Seed& s2) {

      std::array<double, 3> fspoint1, fspoint2;
      std::array<double, 3> fserr1,fserr2;
      std::array<double, 3> fsdir1,fsdir2;
      std::array<double, 3> fsderr1,fsderr2;

      if (!s1.IsValid() || !s2.IsValid() ) {
        throw std::invalid_argument("Cannot compare invalid seeds!");
      }

      s1.GetPoint(fspoint1.data(), fserr1.data());
      s2.GetPoint(fspoint2.data(), fserr2.data());

      s1.GetDirection(fsdir1.data(), fsderr1.data());
      s2.GetDirection(fsdir2.data(), fsderr2.data());

      if (compare(fspoint1.data(), fspoint2.data(), 3) == false) {
        std::cout << "points are different!" << "\n";
        return false; }
      if (compare(fserr1.data(), fserr2.data(), 3) == false) {
        std::cout << "point errors are different!" << "\n";
        return false; }
      if (compare(fsdir1.data(), fsdir2.data(), 3) == false) {
        std::cout << "directions are different!" << "\n";
        return false; }
      if (compare(fsderr1.data(), fsderr2.data(), 3) == false) {
        std::cout << "direction errors are different!" << "\n";
        return false; }
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Seed& s, const unsigned int)
      {
        std::array<double, 3> fspoint;
        std::array<double, 3> fserr;
        std::array<double, 3> fsdir;
        std::array<double, 3> fsderr;

        if (!s.IsValid()) {
          throw std::invalid_argument("seed is not valid!");
        }

        s.GetPoint(fspoint.data(), fserr.data());
        s.GetDirection(fsdir.data(), fsderr.data());

        ar << fspoint;
        ar << fsdir;
        ar << fserr;
        ar << fsderr;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Seed& s, const unsigned int)
      {
        std::array<double, 3> fspoint;
        std::array<double, 3> fserr;
        std::array<double, 3> fsdir;
        std::array<double, 3> fsderr;

        ar >> fspoint;
        ar >> fsdir;
        ar >> fserr;
        ar >> fsderr;

        s = recob::Seed(fspoint.data(), fsdir.data(), fserr.data(), fsderr.data());
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::Seed)
#endif
