#if !defined(hit_serialization_h)
#define hit_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/Hit.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const geo::WireID& wid, const unsigned int)
      {
        unsigned int a = wid.Wire;
        unsigned int b = wid.Plane;
        unsigned int c = wid.TPC;
        unsigned int d = wid.Cryostat;
        // ar << wid.Wire << wid.Plane << wid.TPC << wid.Cryostat;
        ar << a << b << c << d;
      }

    template <class Archive>
      void
      load(Archive& ar, geo::WireID& wid, const unsigned int)
      {
        unsigned int a, b, c, d;
        ar >> a >> b >> c >> d;
        // geo::WireID w(geo::PlaneID(geo::TPCID(geo::CryostatID(d), c), b), a);
        geo::WireID w(d, c, b, a);
        wid = w;
      }

    template <class Archive>
      void
      save(Archive& ar, const recob::Hit& h, const unsigned int)
      {
        ar << h.Channel();
        ar << h.StartTick();
        ar << h.EndTick();
        ar << h.PeakTime();
        ar << h.SigmaPeakTime();
        ar << h.RMS();
        ar << h.PeakAmplitude();
        ar << h.SigmaPeakAmplitude();
        ar << h.SummedADC();
        ar << h.Integral();
        ar << h.SigmaIntegral();
        ar << h.Multiplicity();
        // ar << h.WireID();
        save(ar, h.WireID(), 0u);
        ar << h.View();
        ar << h.SignalType();
        ar << h.DegreesOfFreedom();
        ar << h.GoodnessOfFit();
        ar << h.LocalIndex();
      }

    template <class Archive>
      void
      load(Archive& ar, recob::Hit& h, const unsigned int version)
      {
        raw::ChannelID_t channel;
        raw::TDCtick_t start_tick;
        raw::TDCtick_t end_tick;
        float peak_time;
        float sigma_peak_time;
        float rms;
        float peak_amplitude;
        float sigma_peak_amplitude;
        float summedADC;
        float hit_integral;
        float hit_sigma_integral;
        short int multiplicity;
        short int local_index;
        float goodness_of_fit;
        int dof;
        geo::View_t view;
        geo::SigType_t signal_type;
        geo::WireID wireID;

        ar >> channel;
        ar >> start_tick;
        ar >> end_tick;
        ar >> peak_time;
        ar >> sigma_peak_time;
        ar >> rms;
        ar >> peak_amplitude;
        ar >> sigma_peak_amplitude;
        ar >> summedADC;
        ar >> hit_integral;
        ar >> hit_sigma_integral;
        ar >> multiplicity;
        // ar >> wireID;
        load(ar, wireID, 0u);
        ar >> view;
        ar >> signal_type;
        ar >> dof;
        ar >> goodness_of_fit;
        ar >> local_index;

        recob::Hit h2(channel,
            start_tick,
            end_tick,
            peak_time,
            sigma_peak_time,
            rms,
            peak_amplitude,
            sigma_peak_amplitude,
            summedADC,
            hit_integral,
            hit_sigma_integral,
            multiplicity,
            local_index,
            goodness_of_fit,
            dof,
            view,
            signal_type,
            wireID);

        h = h2;
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(geo::WireID)
BOOST_SERIALIZATION_SPLIT_FREE(recob::Hit)
#endif
