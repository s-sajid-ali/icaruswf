#if !defined(ophit_serialization_h)
#define ophit_serialization_h

#include <iostream>

#include <lardataobj/RecoBase/OpHit.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>

namespace recob {

  inline bool
    operator==(const recob::OpHit& o1, const recob::OpHit& o2) {
      if (o1.OpChannel() != o2.OpChannel()) {
        throw std::runtime_error("opchannel values are not same");
        return false;}
      if (o1.Frame() != o2.Frame()) {
        throw std::runtime_error("frame values are not same");
        return false;}
      if (o1.PeakTime() != o2.PeakTime()) {
        throw std::runtime_error("peaktime values are not same");
        return false;}
      if (o1.PeakTimeAbs() != o2.PeakTimeAbs()) {
        throw std::runtime_error("peaktimeabs values are not same");
        return false;}
      if (o1.Width() != o2.Width()) {
        throw std::runtime_error("ftt values are not same");
        return false;}
      if (o1.Area() != o2.Area()) {
        throw std::runtime_error("area values are not same");
        return false;}
      if (o1.Amplitude() != o2.Amplitude()) {
        throw std::runtime_error("amplitude values are not same");
        return false;}
      if (o1.PE() != o2.PE()) {
        throw std::runtime_error("pe values are not same");
        return false;}
      if (o1.FastToTotal() != o2.FastToTotal()) {
        throw std::runtime_error("fasttotoal values are not same");
        return false;}
      return true;
    }
}

namespace boost {
  namespace serialization {
    template <class Archive>
      void
      save(Archive& ar, const recob::OpHit& o, const unsigned int)
      {
        int                   channel = o.OpChannel();
        unsigned short        frame = o.Frame();
        double                pt = o.PeakTime();
        double                ptabs = o.PeakTimeAbs();
        double                width = o.Width();
        double                area = o.Area();
        double                amp = o.Amplitude();
        double                pe = o.PE();
        double                ftt = o.FastToTotal();

        ar << channel << frame << pt << ptabs << width << area << amp << pe << ftt;

      }

    template <class Archive>
      void
      load(Archive& ar, recob::OpHit& o, const unsigned int)
      {
        int                   channel;
        unsigned short        frame;
        double                pt;
        double                ptabs;
        double                width;
        double                area;
        double                amp;
        double                pe;
        double                ftt;

        ar >> channel >> frame >> pt >> ptabs >> width >> area >> amp >> pe >> ftt;

        o = recob::OpHit(channel, pt, ptabs, frame, width, area, amp, pe, ftt);

      }
  }
}
BOOST_SERIALIZATION_SPLIT_FREE(recob::OpHit)
#endif
