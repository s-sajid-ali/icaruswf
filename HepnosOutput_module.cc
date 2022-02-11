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

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "hepnos.hpp"

#include "hit_serialization.h"
#include "rawdigit_serialization.h"
#include "spacepoint_serialization.h"
#include "wire_serialization.h"

#include <boost/serialization/utility.hpp>
#include "HepnosDataStore.h"

using namespace art;
namespace hepnos { 
  template <typename A, typename B>
  using Assns =std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>>; 
}

namespace {
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
    //std::cout << a.first.key() << "," << a.second.key() << "\n";
    auto const A_ptr = datastore.makePtr<A>(a_map.at(a.id()), a.key());
    auto const B_ptr = datastore.makePtr<B>(a_map.at(b.id()), b.key());
    h_assns.push_back(std::make_pair(A_ptr, B_ptr)); 
  }
    h_e.store(a_t, h_assns); 
}

} // namespace

namespace icaruswf {
  class HepnosOutput;
}

class HepnosOutput : public OutputModule {
public:
  struct Config {
    fhicl::TableFragment<OutputModule::Config> omConfig;
    fhicl::Atom<bool> resolveProducts{fhicl::Name("resolveProducts"), true};
  };

  using Parameters =
    fhicl::WrappedTable<Config, OutputModule::Config::KeysToIgnore>;

  explicit HepnosOutput(Parameters const&);

private:
  void write(EventPrincipal& e) override;
  void writeRun(RunPrincipal& r) override {}
  void writeSubRun(SubRunPrincipal& sr) override{}
  void beginRun(RunPrincipal const& r) override;
  void beginSubRun(SubRunPrincipal const& sr) override;
  bool const wantResolveProducts_;
  hepnos::Run r_;
  hepnos::SubRun sr_;
  hepnos::DataStore & datastore_;
  hepnos::DataSet dataset_;
  std::map<art::ProductID, hepnos::ProductID> translator_;
}; // HepnosOutput

HepnosOutput::HepnosOutput(Parameters const& ps)
  : OutputModule{ps().omConfig, ps.get_PSet()}
  , wantResolveProducts_{ps().resolveProducts()}
  , datastore_{art::ServiceHandle<icaruswf::HepnosDataStore>()->getStore()}
  , dataset_{datastore_.root().createDataSet(ps().omConfig().fileName())}
{
}


void
HepnosOutput::beginRun(RunPrincipal const& r)
{
  r_ = dataset_.createRun(r.run());
}

void
HepnosOutput::beginSubRun(SubRunPrincipal const& sr)
{
  sr_ = r_.createSubRun(sr.subRun());
}

void
HepnosOutput::write(EventPrincipal& p)
{
  if (!p.size())
    return;
  hepnos::Event h_e = sr_.createEvent(p.event());
  //need to make sure we have the map with art::ProductIDs and 
  //hepnos::ProductIDs before we attempt storing association 
  //collections in hepnos store, otherwise since products may be
  //in different order, we may get into a situation where we 
  //dont have both productIDs for association collection
  //So this first for loop is to populate the map, and store all 
  //the products, and the next for loop is for all the assns.
  for (auto const& pr : p) {
    auto const& g = *pr.second;
    auto const& pd = g.productDescription();
    auto const& oh = p.getForOutput(pd.productID(), wantResolveProducts_);
    std::cout << pd.inputTag() << std::endl;
    //dynamic cast to the type we care about is needed here
    EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
    if (auto pwt = prodWithType<std::vector<raw::RawDigit>>(product, pd)) 
       translator_[pd.productID()] = h_e.store(pd.inputTag(), *pwt); 
    else if (auto pwt = prodWithType<std::vector<recob::Wire>>(product, pd)) 
       translator_[pd.productID()] = h_e.store(pd.inputTag(), *pwt); 
    else if (auto pwt = prodWithType<std::vector<recob::Hit>>(product, pd)) 
       translator_[pd.productID()] = h_e.store(pd.inputTag(), *pwt); 
    else if (auto pwt = prodWithType<std::vector<recob::SpacePoint>>(product, pd)) 
       translator_[pd.productID()] = h_e.store(pd.inputTag(), *pwt); 
       }
  
   for (auto const& pr : p) {
    auto const& g = *pr.second;
    auto const& pd = g.productDescription();
    auto const& oh = p.getForOutput(pd.productID(), wantResolveProducts_);
    
    //dynamic cast to the type we care about is needed here
    EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
    if (auto pwt = prodWithType<art::Assns<raw::RawDigit,recob::Wire,void>>(product, pd)) 
       storeassns(datastore_, h_e, translator_, pd.inputTag(), *pwt); 
    else if (auto pwt = prodWithType<art::Assns<recob::Hit,recob::Wire,void>>(product, pd)) { 
      std::cout << "Store Assns: Hit/Wire\n"; 
      storeassns(datastore_, h_e, translator_, pd.inputTag(), *pwt); 
   }
    else if (auto pwt = prodWithType<art::Assns<recob::Wire,recob::Hit,void>>(product, pd)) { 
      std::cout << "Store Assns: Hit/Wire\n"; 
      storeassns(datastore_, h_e, translator_, pd.inputTag(), *pwt); 
   }
   }
}


DEFINE_ART_MODULE(HepnosOutput)
