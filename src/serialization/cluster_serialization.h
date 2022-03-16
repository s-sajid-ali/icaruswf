
#if !defined(cluster_serialization_h)
#define cluster_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/Cluster.h>
#include <larcoreobj/SimpleTypesAndConstants/geo_types.h>

#include <boost/serialization/array.hpp>

namespace recob {
  inline bool
    operator==(const recob::Cluster& c1, const recob::Cluster& c2) {

      if (c1.NHits() != c2.NHits()) {
        throw std::runtime_error("nhits are not same");
        return false;}
      if (c1.StartWire() != c2.StartWire()) {
        throw std::runtime_error("start wires are not same");
        return false;}
      if (c1.EndWire() != c2.EndWire()) {
        throw std::runtime_error("end wires are not same");
        return false;}
      if (c1.SigmaStartWire() != c2.SigmaStartWire()) {
        throw std::runtime_error("sigma start wires are not same");
        return false;}
      if (c1.SigmaEndWire() != c2.SigmaEndWire()) {
        throw std::runtime_error("sigma end wires are not same");
        return false;}
      if (c1.StartTick() != c2.StartTick()) {
        throw std::runtime_error("start ticks are not same");
        return false;}
      if (c1.EndTick() != c2.EndTick()) {
        throw std::runtime_error("end ticks are not same");
        return false;}
      if (c1.SigmaStartTick() != c2.SigmaStartTick()) {
        throw std::runtime_error("sigma start ticks are not same");
        return false;}
      if (c1.SigmaEndTick() != c2.SigmaEndTick()) {
        throw std::runtime_error("sigma end ticks are not same");
        return false;}
      if (c1.StartCharge() != c2.StartCharge()) {
        throw std::runtime_error("start charges are not same");
        return false;}
      if (c1.EndCharge() != c2.EndCharge()) {
        throw std::runtime_error("end charges are not same");
        return false;}
      if (c1.StartAngle() != c2.StartAngle()) {
        throw std::runtime_error("start angles are not same");
        return false;}
      if (c1.EndAngle() != c2.EndAngle()) {
        throw std::runtime_error("end angles are not same");
        return false;}
      if (c1.StartOpeningAngle() != c2.StartOpeningAngle()) {
        throw std::runtime_error("start oangles are not same");
        return false;}
      if (c1.EndOpeningAngle() != c2.EndOpeningAngle()) {
        throw std::runtime_error("end oangles are not same");
        return false;}
      if (c1.Integral() != c2.Integral()) {
        throw std::runtime_error("integrals are not same");
        return false;}
      if (c1.SummedADC() != c2.SummedADC()) {
        throw std::runtime_error("summed adcs are not same");
        return false;}
      if (c1.IntegralStdDev() != c2.IntegralStdDev()) {
        throw std::runtime_error("integral stddevs are not same");
        return false;}
      if (c1.SummedADCstdDev() != c2.SummedADCstdDev()) {
        throw std::runtime_error("summed adc stddevs are not same");
        return false;}
      if (c1.MultipleHitDensity() != c2.MultipleHitDensity()) {
        throw std::runtime_error("multiple hit densities are not same");
        return false;}
      if (c1.Width() != c2.Width()) {
        throw std::runtime_error("widths are not same");
        return false;}
      if (c1.ID() != c2.ID()) {
        throw std::runtime_error("IDs are not same");
        return false;}
      if (c1.View() != c2.View()) {
        throw std::runtime_error("views are not same");
        return false;}
      if (c1.Plane().Plane != c2.Plane().Plane) {
        throw std::runtime_error("pid plane ids are not same");
        return false;}
      if (c1.Plane().TPC != c2.Plane().TPC) {
        throw std::runtime_error("pid tpc ids are not same");
        return false;}
      if (c1.Plane().Cryostat != c2.Plane().Cryostat) {
        throw std::runtime_error("pid cryostat ids are not same");
        return false;}

      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::Cluster& c, const unsigned int)
      {

        unsigned int nhits        = c.NHits();
        float        wire_start   = c.StartWire();
        float        wire_end     = c.EndWire();
        float        sigmaw_start = c.SigmaStartWire();
        float        sigmaw_end   = c.SigmaEndWire();
        float        tick_start   = c.StartTick();
        float        tick_end     = c.EndTick();
        float        sigmat_start = c.SigmaStartTick();
        float        sigmat_end   = c.SigmaEndTick();
        float        charge_start = c.StartCharge();
        float        charge_end   = c.EndCharge();
        float        angle_start  = c.StartAngle();
        float        angle_end    = c.EndAngle();
        float        oangle_start = c.StartOpeningAngle();
        float        oangle_end   = c.EndOpeningAngle();
        float        integral     = c.Integral();
        float        sumadc       = c.SummedADC();
        float        int_stddev   = c.IntegralStdDev();
        float        sadc_stddev  = c.SummedADCstdDev();
        float        mhit_density = c.MultipleHitDensity();
        float        width        = c.Width();
        int          idt          = c.ID();
        geo::View_t  view         = c.View();
        unsigned int pid_planeid  = c.Plane().Plane;
        unsigned int pid_tpcid    = c.Plane().TPC;
        unsigned int pid_tpccryo  = c.Plane().Cryostat;

        ar << nhits;
        ar << wire_start;
        ar << wire_end;
        ar << sigmaw_start;
        ar << sigmaw_end;
        ar << tick_start;
        ar << tick_end;
        ar << sigmat_start;
        ar << sigmat_end;
        ar << charge_start;
        ar << charge_end;
        ar << angle_start;
        ar << angle_end;
        ar << oangle_start;
        ar << oangle_end;
        ar << integral;
        ar << sumadc;
        ar << int_stddev;
        ar << sadc_stddev;
        ar << mhit_density;
        ar << width;
        ar << idt;
        ar << view;
        ar << pid_planeid;
        ar << pid_tpcid;
        ar << pid_tpccryo;
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Cluster& c, const unsigned int)
      {
        unsigned int nhits;
        float        wire_start;
        float        wire_end;
        float        sigmaw_start;
        float        sigmaw_end;
        float        tick_start;
        float        tick_end;
        float        sigmat_start;
        float        sigmat_end;
        float        charge_start;
        float        charge_end;
        float        angle_start;
        float        angle_end;
        float        oangle_start;
        float        oangle_end;
        float        integral;
        float        sumadc;
        float        int_stddev;
        float        sadc_stddev;
        float        mhit_density;
        float        width;
        int          idt;
        geo::View_t  view;
        unsigned int pid_planeid;
        unsigned int pid_tpcid;
        unsigned int pid_tpccryo;

        ar >> nhits;
        ar >> wire_start;
        ar >> wire_end;
        ar >> sigmaw_start;
        ar >> sigmaw_end;
        ar >> tick_start;
        ar >> tick_end;
        ar >> sigmat_start;
        ar >> sigmat_end;
        ar >> charge_start;
        ar >> charge_end;
        ar >> angle_start;
        ar >> angle_end;
        ar >> oangle_start;
        ar >> oangle_end;
        ar >> integral;
        ar >> sumadc;
        ar >> int_stddev;
        ar >> sadc_stddev;
        ar >> mhit_density;
        ar >> width;
        ar >> idt;
        ar >> view;
        ar >> pid_planeid;
        ar >> pid_tpcid;
        ar >> pid_tpccryo;

        c = recob::Cluster(wire_start, sigmaw_start, tick_start, sigmat_start, charge_start, angle_start, oangle_start,
            wire_end, sigmaw_end, tick_end, sigmat_end, charge_end, angle_end, oangle_end,
            integral, int_stddev, sumadc, sadc_stddev, nhits, mhit_density, width, idt,
            view, static_cast<const geo::PlaneID&>(geo::PlaneID(pid_tpccryo, pid_tpcid, pid_planeid)));
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::Cluster)
#endif
