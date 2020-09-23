#include "lardataobj/RecoBase/Hit.h"

namespace recob {
  inline bool
  operator==(Hit const& a, Hit const& b)
  {
    return (a.Channel() == b.Channel())
     && (a.View() == b.View())
     && (a.StartTick() == b.StartTick())
     && (a.EndTick() == b.EndTick())
     && (a.PeakTime() == b.PeakTime())
     && (a.SigmaPeakTime() == b.SigmaPeakTime())
     && (a.RMS() == b.RMS())
     && (a.PeakAmplitude() == b.PeakAmplitude())
     && (a.SigmaPeakAmplitude() == b.SigmaPeakAmplitude())
     && (a.SummedADC() == b.SummedADC())
     && (a.Integral() == b.Integral())
     && (a.SigmaIntegral() == b.SigmaIntegral())
     && (a.Multiplicity() == b.Multiplicity())
     && (a.LocalIndex() == b.LocalIndex())
     && (a.GoodnessOfFit() == b.GoodnessOfFit())
     && (a.DegreesOfFreedom() == b.DegreesOfFreedom())
     && (a.SignalType() == b.SignalType())
     && (a.WireID() == b.WireID());
      
  }
}
