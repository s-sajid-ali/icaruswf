#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/OutputModule.h"
#include "art/Framework/Principal/EventPrincipal.h"
#include "art/Framework/Principal/ResultsPrincipal.h"
#include "art/Framework/Principal/RunPrincipal.h"
#include "art/Framework/Principal/SubRunPrincipal.h"
#include "art/Framework/Services/Registry/ServiceHandle.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Common/Wrapper.h"
#include "fhiclcpp/types/ConfigurationTable.h"
#include "canvas/Persistency/Provenance/canonicalProductName.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "hepnos.hpp"

#include "../serialization/cluster_serialization.h"
#include "../serialization/edge_serialization.h"
#include "../serialization/hit_serialization.h"
#include "../serialization/opdetwaveform_serialization.h"
#include "../serialization/opflash_serialization.h"
#include "../serialization/ophit_serialization.h"
#include "../serialization/hit_serialization.h"
#include "../serialization/pcaxis_serialization.h"
#include "../serialization/pfparticle_serialization.h"
#include "../serialization/rawdigit_serialization.h"
#include "../serialization/seed_serialization.h"
#include "../serialization/slice_serialization.h"
//#include "../serialization/shower_serialization.h"
#include "../serialization/spacepoint_serialization.h"
#include "../serialization/track_serialization.h"
#include "../serialization/vertex_serialization.h"
#include "../serialization/wire_serialization.h"

#include <boost/serialization/utility.hpp>
#include "HepnosDataStore.h"
#include "canvas/Utilities/uniform_type_name.h"

#include "canvas/Utilities/FriendlyName.h"

using namespace art;
namespace hepnos {
  template <typename A, typename B>
    using Assns =std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>>;
  template <typename A, typename B, typename D>
    using AssnsD =std::vector<std::tuple<hepnos::Ptr<A>, hepnos::Ptr<B>, D>>;
}

namespace {

  std::tuple<std::string, std::string, std::string> splitTag(std::string const& inputtag) {
    //InputTag: label = 'daq0', instance = 'PHYSCRATEDATATPCEE', process = 'DetSim'
    //For clarity I am defining all these variables
    std::string label = "label = '";
    std::string instance = "instance = '";
    std::string process = "process = '";
    auto ll = label.length();
    auto il = instance.length();
    auto pl = process.length();
    auto ls = inputtag.find(label);
    auto is = inputtag.find(instance);
    auto ps = inputtag.find(process);
    return std::make_tuple(inputtag.substr(ls+ll, (is-(ls+ll)-3)), inputtag.substr(is+il, ps-(il+is)-3),inputtag.substr(ps+pl, inputtag.length()-(ps+pl)-1));
  }

  template <typename P>
    P const * prodWithType(art::EDProduct const* product, art::BranchDescription const& pd) {
      if (product == nullptr) return nullptr;
      if (pd.producedClassName() != art::TypeID{typeid(P)}.className()) return nullptr;
      if (auto wp = dynamic_cast<art::Wrapper<P> const*>(product))
        return wp->product();
      return nullptr;
    }


  template<typename A, typename B>
    void
    storeassns(hepnos::DataStore datastore,
        hepnos::Event & h_e,
        std::map<art::ProductID, hepnos::ProductID> const& a_map,
        art::InputTag const& a_t,
        art::Assns<A, B> const& a_assns
        )
    {
      hepnos::Assns<A, B> h_assns;
      h_assns.reserve(a_assns.size());

      for (auto const & [a,b]: a_assns) {
        auto const A_ptr = datastore.makePtr<A>(a_map.at(a.id()), a.key());
        auto const B_ptr = datastore.makePtr<B>(a_map.at(b.id()), b.key());
        h_assns.emplace_back(A_ptr, B_ptr);
      }
      h_e.store(a_t, h_assns);
      //std::cout << "Assns in art event: " << a_t.process() << ", " << a_t.label() << ", " << a_t.instance() << "\n";
    }

  template<typename A, typename B, typename D>
    void
    storeassns(hepnos::DataStore datastore,
        hepnos::Event & h_e,
        std::map<art::ProductID, hepnos::ProductID> const& a_map,
        art::InputTag const& a_t,
        art::Assns<A, B, D> const& a_assns
        )
    {
      hepnos::AssnsD<A, B, D> h_assns;
      h_assns.reserve(a_assns.size());

      for (auto const & [a,b, d]: a_assns) {
        auto const A_ptr = datastore.makePtr<A>(a_map.at(a.id()), a.key());
        auto const B_ptr = datastore.makePtr<B>(a_map.at(b.id()), b.key());
        h_assns.emplace_back(A_ptr, B_ptr, d);
      }
      h_e.store(a_t, h_assns);
      //std::cout << "Assns in art event: " << a_t.process() << ", " << a_t.label() << ", " << a_t.instance() << "\n";
    }
  
  std::map<art::ProductID, hepnos::ProductID>
    update_map_hevent(hepnos::Event h_e)
    {
      std::map<art::ProductID, hepnos::ProductID> translator;
      auto prodIds = h_e.listProducts("");
      hepnos::UUID datasetId;
      hepnos::RunNumber run;
      hepnos::SubRunNumber subrun;
      hepnos::EventNumber hepnosevent;
      std::string hepnostag;
      std::string type;
      //for data products in hepnos event
      for (auto p: prodIds) {
        p.unpackInformation(&datasetId, &run, &subrun, &hepnosevent, &hepnostag, &type);
        auto [label, instance, processname] = splitTag(hepnostag);
        auto t = art::uniform_type_name(type);
        auto const product_name = art::canonicalProductName(art::friendlyname::friendlyName(t),
            label,
            instance,
            processname);
        auto art_pid = art::ProductID{product_name};
        if (translator.find(art_pid) != translator.cend()) continue;
        translator[art_pid] = p;
        //std::cout << "hepnos event: " << art_pid << ", " << product_name << "\n";
      }
      return translator;
    }

  std::map<art::ProductID, hepnos::ProductID>
    update_map_aevent(EventPrincipal& a_e, hepnos::Event h_e) {
      std::map<art::ProductID, hepnos::ProductID> translator;
      for (auto const& pr : a_e) {
        auto const& g = *pr.second;
        auto const& pd = g.productDescription();
        auto const& oh = a_e.getForOutput(pd.productID(), true);
        //std::cout << "art event: " << pd.productID() << ", " << pd.branchName() << ", " << pd.processName()<< "\n";
        //dynamic cast to the type we care about is needed here
        EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
        if (auto pwt = prodWithType<std::vector<raw::RawDigit>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "DetSim");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For wires as output of signal processing and input for hit finding
        if (auto pwt = prodWithType<std::vector<recob::Wire>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "SignalProcessing");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For hits as output of hit finding
        // Hits are also output of Pandora?
        if (auto pwt = prodWithType<std::vector<recob::Hit>>(product, pd)) {
          translator[pd.productID()] = h_e.store(pd.inputTag(), *pwt);
        }
        // For SpacePoints as output of Pandora and MCstage1
        if (auto pwt = prodWithType<std::vector<recob::SpacePoint>>(product, pd)) {
          translator[pd.productID()] = h_e.store(pd.inputTag(), *pwt);
        }
        // For Edge as output of Pandora
        if (auto pwt = prodWithType<std::vector<recob::Edge>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "Pandora");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For PFParticles as output of Pandora and MCStage1
        if (auto pwt = prodWithType<std::vector<recob::PFParticle>>(product, pd)) {
          translator[pd.productID()] = h_e.store(pd.inputTag(), *pwt);
        }
        // For Seeds as output of Pandora
        if (auto pwt = prodWithType<std::vector<recob::Seed>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "Pandora");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For Slices as output of Pandora
        if (auto pwt = prodWithType<std::vector<recob::Slice>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "Pandora");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For Vertices as output of Pandora
        if (auto pwt = prodWithType<std::vector<recob::Vertex>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "Pandora");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For Clusters as output of Pandora
        if (auto pwt = prodWithType<std::vector<recob::Cluster>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "Pandora");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        // For PCAxis as output of Pandora and MCstage1
        if (auto pwt = prodWithType<std::vector<recob::PCAxis>>(product, pd)) {
          translator[pd.productID()] = h_e.store(pd.inputTag(), *pwt);
        }
        //For OpDetWaveform as output of 
        if (auto pwt = prodWithType<std::vector<raw::OpDetWaveform>>(product, pd)) {
         // auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage0");
          translator[pd.productID()] = h_e.store(pd.inputTag(), *pwt);
        }
        //For OpHits as output of MCstage0
        if (auto pwt = prodWithType<std::vector<recob::OpHit>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage0");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        //For OpFlashs as output of MCstage0
        if (auto pwt = prodWithType<std::vector<recob::OpFlash>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage0");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
        //For Tracks as output of MCstage1
        if (auto pwt = prodWithType<std::vector<recob::Track>>(product, pd)) {
          auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage1");
          translator[pd.productID()] = h_e.store(inputtag, *pwt);
        }
     //   //For TrackHitMeta as output of MCstage1
     //   if (auto pwt = prodWithType<std::vector<recob::TrackHitMeta>>(product, pd)) {
     //     auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage1");
     //     translator[pd.productID()] = h_e.store(inputtag, *pwt);
     //   }
     //   //For Shower as output of MCstage1
     //   if (auto pwt = prodWithType<std::vector<recob::Shower>>(product, pd)) {
     //     auto inputtag = art::InputTag(pd.inputTag().label(), pd.inputTag().instance(), "MCstage1");
     //     translator[pd.productID()] = h_e.store(inputtag, *pwt);
     //   }

      }
      return translator;
    }

  void
    store_aassns(art::EventPrincipal& p,
        hepnos::DataStore ds,
        hepnos::Event h_e,
        std::map<art::ProductID, hepnos::ProductID>& translator) {
      // for associaation collections in art event
      for (auto const& pr : p) {
        auto const& g = *pr.second;
        auto const& pd = g.productDescription();
        if (translator.find(pd.productID()) != translator.cend()) continue;
        auto const& oh = p.getForOutput(pd.productID(), true);

        //dynamic cast to the type we care about is needed here
        EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
        if (auto pwt = prodWithType<art::Assns<raw::RawDigit,recob::Wire,void>>(product, pd))
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        if (auto pwt = prodWithType<art::Assns<recob::Wire,recob::Hit,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Hit,recob::SpacePoint,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::PFParticle,recob::SpacePoint,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Edge,recob::PFParticle,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Edge,recob::SpacePoint,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::PFParticle,recob::Seed,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Seed,recob::Hit,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::PFParticle,recob::Slice,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Hit,recob::Slice,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::PFParticle,recob::Vertex,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Cluster, recob::PFParticle, void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Cluster, recob::Hit,void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::OpFlash, recob::OpHit, void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::Hit, recob::Track, void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
    //    if (auto pwt = prodWithType<art::Assns<recob::Hit, recob::Track, recob::TrackHitMeta>>(product, pd)) {
    //      storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
    //    }
       if (auto pwt = prodWithType<art::Assns<recob::PCAxis, recob::PFParticle, void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        if (auto pwt = prodWithType<art::Assns<recob::PFParticle, recob::Track, void>>(product, pd)) {
          storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
        }
        //all Association collections with Shower
     //  if (auto pwt = prodWithType<art::Assns<recob::Shower, recob::Track, void>>(product, pd)) {
     //     storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //   }
     //  if (auto pwt = prodWithType<art::Assns<recob::PFParticle, recob::Shower, void>>(product, pd)) {
     //     storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //   }
     //  if (auto pwt = prodWithType<art::Assns<recob::Hit, recob::Shower, void>>(product, pd)) {
     //     storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //   }
     //  if (auto pwt = prodWithType<art::Assns<recob::PCAxis, recob::Shower, void>>(product, pd)) {
     //     storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //   }
     //  if (auto pwt = prodWithType<art::Assns<recob::Cluster, recob::Shower, void>>(product, pd)) {
     //     storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //   }
     //  if (auto pwt = prodWithType<art::Assns<recob::Shower, recob::SpacePoint, void>>(product, pd)) {
     //    storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
     //  }
      }
    }
} // namespace

namespace icaruswf {
  class HepnosSPOutput;
}

class HepnosSPOutput : public OutputModule {
  public:
    struct Config {
      fhicl::TableFragment<OutputModule::Config> omConfig;
      fhicl::Atom<bool> forwardProducts{fhicl::Name("forwardProducts"), false};
    };

    using Parameters =
      fhicl::WrappedTable<Config, OutputModule::Config::KeysToIgnore>;

    explicit HepnosSPOutput(Parameters const&);

  private:
    void write(EventPrincipal& e) override;
    void writeRun(RunPrincipal& r) override {}
    void writeSubRun(SubRunPrincipal& sr) override{}
    void beginRun(RunPrincipal const& r) override;
    void beginSubRun(SubRunPrincipal const& sr) override;
    bool const wantForwardProducts_;
    hepnos::Run r_;
    hepnos::SubRun sr_;
    hepnos::DataStore & datastore_;
    hepnos::DataSet dataset_;
    std::map<art::ProductID, hepnos::ProductID> translator_;
}; // HepnosSPOutput

HepnosSPOutput::HepnosSPOutput(Parameters const& ps)
  : OutputModule{ps().omConfig, ps.get_PSet()}
  , wantForwardProducts_{ps().forwardProducts()}
  , datastore_{art::ServiceHandle<icaruswf::HepnosDataStore>()->getStore()}
  , dataset_{datastore_.root().createDataSet(ps().omConfig().fileName())}
{
}


  void
HepnosSPOutput::beginRun(RunPrincipal const& r)
{
  r_ = dataset_.createRun(r.run());
}

  void
HepnosSPOutput::beginSubRun(SubRunPrincipal const& sr)
{
  sr_ = r_.createSubRun(sr.subRun());
}

  void
HepnosSPOutput::write(EventPrincipal& p)
{
  if (!p.size())
    return;
  hepnos::Event h_e = sr_.createEvent(p.event());
  auto map1 = update_map_hevent(h_e);
  translator_.insert(map1.begin(), map1.end());
  auto map2 = update_map_aevent(p, h_e);
  translator_.insert(map2.begin(), map2.end());
  store_aassns(p, datastore_, h_e, translator_);

}


DEFINE_ART_MODULE(HepnosSPOutput)
