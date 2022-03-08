#include <iostream>
#include <utility>

#include <lardataobj/RecoBase/PCAxis.h>

#include <boost/serialization/array.hpp>
#include <boost/serialization/vector.hpp>

#include "utils.h"

namespace recob {
  inline bool
    operator==(const recob::PCAxis& p1, const recob::PCAxis& p2) {
      if (p1.getSvdOK() != p2.getSvdOK()) return false;
      if (p1.getNumHitsUsed() != p2.getNumHitsUsed()) return false;
      if (p1.getAvePosition() != p2.getAvePosition()) return false;
      if (p1.getAveHitDoca() != p2.getAveHitDoca()) return false;
      if (p1.getID() != p2.getID()) return false;
      if (compare(p1.getEigenValues(), p2.getEigenValues(), 3) == false) {
        std::cout << "eigen values are different!" << "\n";
        return false;}
      PCAxis::EigenVectors eigvecs1, eigvecs2;
      eigvecs1 = p1.getEigenVectors();
      eigvecs2 = p2.getEigenVectors();
      if (eigvecs1.size()!=3 || eigvecs2.size()!=3) {
        throw std::invalid_argument("Invalid eigenvectors! \
            There should only be three eigenvectors.");
      }
      for (int i=0; i<3; i++) {
        if (eigvecs1[i].size()!=3 || eigvecs2[i].size()!=3) {
          throw std::invalid_argument("Invalid eigenvectors! \
              Each eigenvector should only have three components.");
        }
        if (compare(eigvecs1[i],eigvecs2[i] , 3) == false ) {
          std::cout << "eigen vector components are different!" << "\n";
          return false;}
      }
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::PCAxis& p, const unsigned int)
      {
        bool   svdstatus  = p.getSvdOK();
        int    nhits      = p.getNumHitsUsed();
        double hitdoca    = p.getAveHitDoca();
        size_t fid        = p.getID();
        std::array<double, 3> eigvals;
        std::array<double, 3> avpos;
        for (int i=0; i<3; i++) {
          eigvals[i] = p.getEigenValues()[i];
          avpos[i]   = p.getAvePosition()[i];
        }
        recob::PCAxis::EigenVectors eigvecs;
        eigvecs = p.getEigenVectors();
        if (eigvecs.size()!=3) {
          throw std::invalid_argument("Invalid eigenvectors! \
              There should only be three eigenvectors.");
        }
        for (int i=0; i<3; i++) {
          if (eigvecs.size()!=3) {
            throw std::invalid_argument("Invalid eigenvectors! \
                Each eigenvector should only have three components.");
          }
        }

        ar << svdstatus;
        ar << nhits;
        ar << avpos;
        ar << hitdoca;
        ar << fid;
        ar << eigvals;
        ar << eigvecs;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::PCAxis& p, const unsigned int)
      {
        bool   svdstatus;
        int    nhits;
        double hitdoca;
        size_t fid;
        std::array<double, 3> avpos;
        std::array<double, 3> eigvals;
        std::array<std::array<double,3>,3> _eigvecs;
        recob::PCAxis::EigenVectors eigvecs;

        eigvecs.resize(3);
        for (int i=0; i<3; i++){
          eigvecs[i].resize(3);
        }

        ar >> svdstatus;
        ar >> nhits;
        ar >> avpos;
        ar >> hitdoca;
        ar >> fid;
        ar >> eigvals;
        ar >> eigvecs;

        p = recob::PCAxis(svdstatus, nhits, static_cast<const double*>(eigvals.data()),
            static_cast<const recob::PCAxis::EigenVectors&>(eigvecs),
            static_cast<const double*>(avpos.data()), static_cast<const double>(hitdoca), fid);
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::PCAxis)

