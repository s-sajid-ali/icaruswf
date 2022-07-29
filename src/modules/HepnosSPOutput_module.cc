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

#include <spdlog/spdlog.h>

#include <algorithm>
#include <chrono>
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
#include "../serialization/spacepoint_serialization.h"
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
    art::InputTag a_inputtag;
    decode(inputtag, a_inputtag);

    std::string label = a_inputtag.label();
    std::string instance = a_inputtag.instance();
    std::string process = a_inputtag.process();
    return std::make_tuple(label, instance, process);
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
      h_e.store(a_t.encode(), h_assns);
      //std::cout << "Assns in art event: " << a_t.process() << ", " << a_t.label() << ", " << a_t.encode() << "\n";
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
      //std::cout << "Assns in art event: " << a_t.process() << ", " << a_t.label() << ", " << a_t.encode() << "\n";
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
      fhicl::Atom<bool> async{fhicl::Name("async"), true};
      fhicl::Atom<bool> batch{fhicl::Name("batch"), true};
    };

    using Parameters =
      fhicl::WrappedTable<Config, OutputModule::Config::KeysToIgnore>;

    explicit HepnosSPOutput(Parameters const&);

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
        hepnos::StoreStatistics stats;
        std::map<art::ProductID, hepnos::ProductID> translator;
        for (auto const& pr : a_e) {
          auto const& g = *pr.second;
          auto const& pd = g.productDescription();
          auto const& oh = a_e.getForOutput(pd.productID(), true);

          //dynamic cast to the type we care about is needed here
          EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
          if (auto pwt = prodWithType<std::vector<raw::RawDigit>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<raw::RawDigit>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For wires as output of signal processing and input for hit finding
          if (auto pwt = prodWithType<std::vector<recob::Wire>>(product, pd)) {
            auto begin = std::chrono::high_resolution_clock::now();
            translator[pd.productID()] = this->store_method<std::vector<recob::Wire>>(h_e, pd.inputTag().encode(), pwt, stats);
            auto end = std::chrono::high_resolution_clock::now();
            auto dur = end - begin;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
            std::cout << "Time for recob::Wire: " << ms << std::endl;
          }
          // For hits as output of hit finding
          // Hits are also output of Pandora?
          if (auto pwt = prodWithType<std::vector<recob::Hit>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Hit>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For SpacePoints as output of Pandora and MCstage1
          if (auto pwt = prodWithType<std::vector<recob::SpacePoint>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::SpacePoint>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For Edge as output of Pandora
          if (auto pwt = prodWithType<std::vector<recob::Edge>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Edge>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For PFParticles as output of Pandora and MCStage1
          if (auto pwt = prodWithType<std::vector<recob::PFParticle>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::PFParticle>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For Seeds as output of Pandora
          if (auto pwt = prodWithType<std::vector<recob::Seed>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Seed>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For Slices as output of Pandora
          if (auto pwt = prodWithType<std::vector<recob::Slice>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Slice>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For Vertices as output of Pandora
          if (auto pwt = prodWithType<std::vector<recob::Vertex>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Vertex>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For Clusters as output of Pandora
          if (auto pwt = prodWithType<std::vector<recob::Cluster>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::Cluster>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          // For PCAxis as output of Pandora and MCstage1
          if (auto pwt = prodWithType<std::vector<recob::PCAxis>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<recob::PCAxis>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
          //For OpDetWaveform as output of
          if (auto pwt = prodWithType<std::vector<raw::OpDetWaveform>>(product, pd)) {
            translator[pd.productID()] = this->store_method<std::vector<raw::OpDetWaveform>>(h_e, pd.inputTag().encode(), pwt, stats);
          }
        }

        spdlog::info("store times: num={}, avg={}, max={}, min={}",
            stats.raw_storage_time.num,
            stats.raw_storage_time.avg,
            stats.raw_storage_time.max,
            stats.raw_storage_time.min);

        spdlog::info("serialization times: num={}, avg={}, max={}, min={}",
            stats.serialization_time.num,
            stats.serialization_time.avg,
            stats.serialization_time.max,
            stats.serialization_time.min);

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
          if (auto pwt = prodWithType<art::Assns<recob::PCAxis, recob::PFParticle, void>>(product, pd)) {
            storeassns(ds, h_e, translator, pd.inputTag(), *pwt);
          }
        }
      }


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
    std::optional<hepnos::AsyncEngine> async_;
    std::optional<hepnos::WriteBatch> batch_;
    hepnos::DataSet dataset_;
    std::map<art::ProductID, hepnos::ProductID> translator_;

    template <typename P>
      hepnos::ProductID store_method(hepnos::Event h_e, const std::string key, P const * pwt, hepnos::StoreStatistics& stats)
      {
        try
        {
          return h_e.store(batch_.value(), key, pwt, &stats);
        }
        catch (const std::bad_optional_access& e)
        {
          try
          {
            return h_e.store(async_.value(), key, pwt, &stats);
          }
          catch (const std::bad_optional_access& e)
          {

            return h_e.store(key, pwt, &stats);
          }
        }
      }

}; // HepnosSPOutput

HepnosSPOutput::HepnosSPOutput(Parameters const& ps)
  : OutputModule{ps().omConfig, ps.get_PSet()}
  , wantForwardProducts_{ps().forwardProducts()}
  , datastore_{art::ServiceHandle<icaruswf::HepnosDataStore>()->getStore()}
  , dataset_{datastore_.root().createDataSet(ps().omConfig().fileName())}
{
  if(ps().async()){
    async_.emplace(hepnos::AsyncEngine(datastore_, 8));
    if (ps().batch()){
      batch_.emplace(hepnos::WriteBatch(async_.value(), 2048));
    }
  }
  else{
    if (ps().batch()){
      batch_.emplace(hepnos::WriteBatch(datastore_, 2048));
    }
  }
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

  auto begin = std::chrono::high_resolution_clock::now();
  auto map1 = this->update_map_hevent(h_e);
  translator_.insert(map1.begin(), map1.end());
  auto end = std::chrono::high_resolution_clock::now();
  auto dur = end - begin;
  auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << "hepnos event products time in ms: " << ms << std::endl;

  begin = std::chrono::high_resolution_clock::now();
  auto map2 = this->update_map_aevent(p, h_e);
  translator_.insert(map2.begin(), map2.end());
  end = std::chrono::high_resolution_clock::now();
  dur = end - begin;
  ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << "art event products time in ms: " << ms << std::endl;

  begin = std::chrono::high_resolution_clock::now();
  this->store_aassns(p, datastore_, h_e, translator_);
  end = std::chrono::high_resolution_clock::now();
  dur = end - begin;
  ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
  std::cout << "art assns time in ms: " << ms << std::endl;
}


DEFINE_ART_MODULE(HepnosSPOutput)
