#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/fwd.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "canvas/Persistency/Common/Assns.h"

#include "art/Framework/Core/ProductRegistryHelper.h"
#include "art/Framework/IO/Sources/SourceHelper.h"
#include "art/Framework/Core/FileBlock.h"

#include "canvas/Persistency/Provenance/RunID.h"
#include "canvas/Persistency/Provenance/SubRunID.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/IO/Sources/put_product_in_principal.h"

#include "fhiclcpp/types/Atom.h"

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

#include "lardataobj/RawData/RawDigit.h"
//#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Wire.h"

#include "hepnos.hpp"

#include "hit_serialization.h"
#include "rawdigit_serialization.h"
#include "spacepoint_serialization.h"
#include "wire_serialization.h"

#include <boost/serialization/utility.hpp>

using namespace std;
namespace hepnos { 
  template <typename A, typename B>
  using Assns =std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>>; 
}

namespace hepnossource{
  class HepnosInputSource {
  public:
    //struct Config {
    //};
  
    //using Parameters = fhicl::Table<Config>;
  
    HepnosInputSource(fhicl::ParameterSet const& p, art::ProductRegistryHelper& rh, art::SourceHelper const& pm) : pm_(pm)
    {
      rh.reconstitutes<std::vector<recob::Hit>, art::InEvent>("gaushit");
    }

    bool readNext(art::RunPrincipal* const& inR,
                  art::SubRunPrincipal* const& inSR,
                  art::RunPrincipal*& outR,
                  art::SubRunPrincipal*& outSR,
                  art::EventPrincipal*& outE);

    void readFile(std::string const& dsname, art::FileBlock*& fb) {
      auto connection_file = "client.yaml";
      auto dataset_name = dsname;
      datastore_ = hepnos::DataStore::connect(connection_file);
      dataset_ = datastore_.root()[dataset_name];
      fb = new art::FileBlock{art::FileFormatVersion{1, "SimpleSource 2017"},
                          dsname};
      es_ = dataset_.events();
      ev_ = es_->begin();
    }

    void closeCurrentFile() {}
  private:
    hepnos::DataStore datastore_;
    hepnos::DataSet dataset_;
    art::SourceHelper const& pm_;
    hepnos::RunNumber r_ = -1ull;
    hepnos::SubRunNumber sr_ = -1ull;
    std::optional<hepnos::EventSet> es_;
    hepnos::EventSet::const_iterator ev_;
  };

    bool
    HepnosInputSource::readNext(art::RunPrincipal* const& inR,
                  art::SubRunPrincipal* const& inSR,
                  art::RunPrincipal*& outR,
                  art::SubRunPrincipal*& outSR,
                  art::EventPrincipal*& outE)
 
    {
      if (ev_ == es_->end()) return false;
      art::Timestamp ts;
      std::cout << ev_->number() << "\n";
      auto sr = ev_->subrun().number();
      auto r = ev_->subrun().run().number();
      if (r != r_) {
        outR = pm_.makeRunPrincipal(r, ts);
        outSR = pm_.makeSubRunPrincipal(r, sr, ts);
        r_ = r;
        sr_ = sr;
      } 
      else if (sr != sr_) {
        outSR = pm_.makeSubRunPrincipal(r, sr, ts);
        sr_ = sr;
      }
      outE = pm_.makeEventPrincipal(r, sr, ev_->number(), ts, false);
      art::InputTag const tag("gaushit", "", "");
      std::vector<recob::Hit> hits;
      ev_->load(tag, hits);
      std::cout << hits.size() << "\n";
      std::unique_ptr<std::vector<recob::Hit>> v1 = std::make_unique<std::vector<recob::Hit>>(hits);
      art::put_product_in_principal(std::move(v1), *outE, "gaushit");
      ++ev_;
      return true;
    }
}
namespace art {
  // We don't want the file services: we must say so by specializing the                                                                                                                                                                 
  //   // template *before* specifying the typedef.                                                                                                                                                                                           
template <>
  struct Source_wantFileServices<hepnossource::HepnosInputSource> {
      static constexpr bool value{false};
        };
        } // namespace art
//typedef art::Source<HepnosInputSource> HepnosInput;
DEFINE_ART_INPUT_SOURCE(art::Source<hepnossource::HepnosInputSource>);
