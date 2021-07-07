
#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/fwd.h"
#include "art/Framework/IO/Sources/Source.h"
#include "art/Framework/Core/InputSourceMacros.h"
#include "canvas/Persistency/Common/Assns.h"
#include "canvas/Persistency/Provenance/canonicalProductName.h"

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

#define private public
#include "hepnos.hpp"
#undef private

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
  template <typename T>
  std::unique_ptr<std::vector<T>> 
  loaddata(hepnos::Event& event, 
           std::string const& module_label) 
  {
     art::InputTag const tag(module_label, "", "");
     std::vector<T> products;
     event.load(tag, products);
     return std::make_unique<std::vector<T>>(products);
  }
  
  template <typename T>
  art::ProductID
  create_productID(std::vector<T> const& t, 
                   std::string const& module_label, 
                   std::string const& instance_name="") 
  {
     auto const product_name = 
             art::canonicalProductName( art::TypeID{t}.friendlyClassName(), 
                                        module_label, 
                                        instance_name, 
                                        art::Globals::instance()->processName());
     return art::ProductID{product_name};      
  } 
 
  template <typename A, typename B>
  std::unique_ptr<art::Assns<A, B>>
  loadassns(hepnos::Event& event, 
            art::EventPrincipal*& outE, 
            std::vector<A> const& a_col, 
            std::vector<B> const& b_col, 
            std::string const& a_module_label, 
            std::string const& b_module_label)
  {
     art::InputTag const tag(a_module_label, "", "");
     std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>> assns;
     event.load(tag, assns);
     art::Assns<A, B> art_assns;
     auto const a_pid = create_productID<A>(a_col, a_module_label, "");
     auto const b_pid = create_productID<B>(b_col, b_module_label, "");
     for (auto const& [a, b]: assns) {
       art_assns.addSingle(art::Ptr<A>(a_pid, a.m_index, outE->productGetter(a_pid)), 
                           art::Ptr<B>(b_pid, b.m_index, outE->productGetter(b_pid)));
     }
     return std::make_unique<art::Assns<A, B>>(art_assns);
  }

  class HepnosInputSource {
  public:
    //struct Config {
    //};
  
    //using Parameters = fhicl::Table<Config>;
  
    HepnosInputSource(fhicl::ParameterSet const& p, 
                      art::ProductRegistryHelper& rh, 
                      art::SourceHelper const& pm) : pm_(pm)
    {
      rh.reconstitutes<std::vector<recob::Hit>, art::InEvent>("gaushit");
      rh.reconstitutes<std::vector<recob::Wire>, art::InEvent>("recowireraw");
      rh.reconstitutes<art::Assns<recob::Hit, recob::Wire>, art::InEvent>("gaushit");
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
      
      auto hits = loaddata<recob::Hit>(*ev_, "gaushit");
      auto wires = loaddata<recob::Wire>(*ev_, "recowireraw");
      auto assns = loadassns<recob::Hit, recob::Wire>(*ev_, outE, *hits, *wires, "gaushit", "recowireraw");
      art::put_product_in_principal(std::move(hits), *outE, "gaushit");
      art::put_product_in_principal(std::move(wires), *outE, "recowireraw");
      art::put_product_in_principal(std::move(assns), *outE, "gaushit");
      ++ev_;
      return true;
    }
}

namespace art {
 // We don't want the file services: we must say so by specializing the             // template *before* specifying the typedef.
  template <>
  struct Source_wantFileServices<hepnossource::HepnosInputSource> {
      static constexpr bool value{false};
  };
} // namespace art
DEFINE_ART_INPUT_SOURCE(art::Source<hepnossource::HepnosInputSource>);
