#if !defined(opflash_serialization_h)
#define opflash_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/OpFlash.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>

#include "utils.h"

namespace recob {

  inline bool
    operator==(const recob::OpFlash& o1, const recob::OpFlash& o2) {
      if (o1.Time() != o2.Time()) {
        throw std::runtime_error("time values are not same");
        return false;}
      if (o1.TimeWidth() != o2.TimeWidth()) {
        throw std::runtime_error("timewidth values are not same");
        return false;}
      if (o1.AbsTime() != o2.AbsTime()) {
        throw std::runtime_error("abstime values are not same");
        return false;}
      if (o1.Frame() != o2.Frame()) {
        throw std::runtime_error("frame values are not same");
        return false;}
      if (o1.XCenter() != o2.XCenter()) {
        throw std::runtime_error("xcenter values are not same");
        return false;}
      if (o1.XWidth() != o2.XWidth()) {
        throw std::runtime_error("xwidth values are not same");
        return false;}
      if (o1.YCenter() != o2.YCenter()) {
        throw std::runtime_error("ycenter values are not same");
        return false;}
      if (o1.YWidth() != o2.YWidth()) {
        throw std::runtime_error("ywidth values are not same");
        return false;}
      if (o1.ZCenter() != o2.ZCenter()) {
        throw std::runtime_error("zcenter values are not same");
        return false;}
      if (o1.ZWidth() != o2.ZWidth()) {
        throw std::runtime_error("zwidth values are not same");
        return false;}
      if (o1.FastToTotal() != o2.FastToTotal()) {
        throw std::runtime_error("ftt values are not same");
        return false;}
      if (o1.InBeamFrame() != o2.InBeamFrame()) {
        throw std::runtime_error("inbeamframe values are not same");
        return false;}
      if (o1.OnBeamTime() != o2.OnBeamTime()) {
        throw std::runtime_error("onbeamtime values are not same");
        return false;}
      if (o1.PEs().size() != o2.PEs().size()) {
        throw std::runtime_error("pes vector sizes are different!");
        return false;}
      if (compare(o1.PEs(), o2.PEs() , o1.PEs().size()) == false ) {
        throw std::runtime_error("pes vector components are different!");
        return false;}
      if (o1.WireCenters().size() != o2.WireCenters().size()) {
        throw std::runtime_error("wirecenters vector sizes are different!");
        return false;}
      if (compare(o1.WireCenters(), o2.WireCenters() , o1.WireCenters().size()) == false ) {
        throw std::runtime_error("wirecenters vector components are different!");
        return false;}
      if (o1.WireWidths().size() != o2.WireWidths().size()) {
        throw std::runtime_error("wirewidths vector sizes are different!");
        return false;}
      if (compare(o1.WireWidths(), o2.WireWidths() , o1.WireWidths().size()) == false ) {
        throw std::runtime_error("wirewidths vector components are different!");
        return false;}

      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::OpFlash& o, const unsigned int)
      {
        double              time = o.Time();
        double              timewidth = o.TimeWidth();
        double              abstime = o.AbsTime();
        unsigned int        frame = o.Frame();
        std::vector<double> peperod = o.PEs();
        std::vector<double> wirecenters = o.WireCenters();
        std::vector<double> wirewidths = o.WireWidths();
        double              xcenter = o.XCenter();
        double              xwidth = o.XWidth();
        double              ycenter = o.YCenter();
        double              ywidth = o.YWidth();
        double              zcenter = o.ZCenter();
        double              zwidth = o.ZWidth();
        double              ftt = o.FastToTotal();
        bool                ibf = o.InBeamFrame();
        int                 obt = o.OnBeamTime();

        ar << time << timewidth << abstime << frame << peperod << wirecenters << wirewidths << xcenter << xwidth << ycenter << ywidth << zcenter << zwidth << ftt << ibf << obt;

      }

    template <class Archive>
      void
      load(Archive& ar, recob::OpFlash& o, const unsigned int)
      {
        double              time;
        double              timewidth;
        double              abstime;
        unsigned int        frame;
        std::vector<double> peperod;
        std::vector<double> wirecenters;
        std::vector<double> wirewidths;
        double              xcenter;
        double              xwidth;
        double              ycenter;
        double              ywidth;
        double              zcenter;
        double              zwidth;
        double              ftt;
        bool                ibf;
        int                 obt;

        ar >> time >> timewidth >> abstime >> frame >> peperod >> wirecenters >> wirewidths >> xcenter >> xwidth >> ycenter >> ywidth >> zcenter >> zwidth >> ftt >> ibf >> obt;

        o = recob::OpFlash(time, timewidth, abstime, frame, peperod, ibf, obt, ftt,
            xcenter, xwidth, ycenter, ywidth, zcenter, zwidth, wirecenters, wirewidths);
      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::OpFlash)
#endif
