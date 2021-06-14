#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/fwd.h"
#include "fhiclcpp/types/Atom.h"

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Wire.h"

template<typename T> 
void 
loaddata(art::Event const& e, const char * modulelabel, const char *instancename="") {
  art::Handle<T> h;
  e.getByLabel(modulelabel, instancename, h);
  if (!h.isValid()) std::cerr << "Missing products for " << modulelabel << ", " << instancename << '\n';
} 

namespace {
  class MergeDataProds : public art::SharedAnalyzer {
  public:
    struct Config {
    };
    using Parameters = Table<Config>;
    explicit MergeDataProds(Parameters const& p, art::ProcessingFrame const&)
      : SharedAnalyzer{p}
    {
      async<art::InEvent>();
    }

  private:
    void
    analyze(art::Event const& e , art::ProcessingFrame const&) override
    {
      std::cout << "Processing events " << e.event() << '\n';
      loaddata<std::vector<raw::RawDigit>>(e, "daq");
      loaddata<std::vector<raw::RawDigit>>(e,"rawdigitfilter");
      loaddata<std::vector<recob::Wire>>(e, "recowireraw");
      loaddata<std::vector<recob::Wire>>(e, "decon1droi");
      loaddata<art::Assns<raw::RawDigit,recob::Wire>>(e, "recowireraw");
      loaddata<art::Assns<raw::RawDigit, recob::Wire>>(e, "decon1droi");
      
      loaddata<std::vector<recob::SpacePoint>>(e, "pandoraGaus");
      loaddata<std::vector<recob::SpacePoint>>(e, "pandoraICARUS");
      loaddata<std::vector<recob::SpacePoint>>(e, "pandoraKalmanTrackICARUS");
      loaddata<std::vector<recob::SpacePoint>>(e, "pandoraKalmanTrackGaus");
      
      loaddata<art::Assns<recob::Hit, recob::SpacePoint>>(e, "pandoraGaus");
      loaddata<art::Assns<recob::Hit, recob::SpacePoint>>(e, "pandoraICARUS");
      loaddata<art::Assns<recob::Hit, recob::SpacePoint>>(e, "pandoraKalmanTrackICARUS");
      loaddata<art::Assns<recob::Hit, recob::SpacePoint>>(e, "pandoraKalmanTrackGaus");
      
      loaddata<std::vector<recob::Hit>>(e, "gaushitall");
      loaddata<std::vector<recob::Hit>>(e, "gaushit");
      loaddata<std::vector<recob::Hit>>(e, "icarushit");
      
      loaddata<art::Assns<recob::Hit, recob::Wire>>(e, "gaushitall");
      loaddata<art::Assns<recob::Hit, recob::Wire>>(e, "icarushit");
      loaddata<art::Assns<recob::Hit, recob::Wire>>(e, "gaushit");

    }

  };
}

DEFINE_ART_MODULE(MergeDataProds)
