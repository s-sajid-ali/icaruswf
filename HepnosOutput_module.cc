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

#include "hit_serialization.h"
#include "rawdigit_serialization.h"
#include "spacepoint_serialization.h"
#include "wire_serialization.h"

#include <boost/serialization/utility.hpp>
#include "HepnosDataStore.h"
#include "canvas/Utilities/uniform_type_name.h"

#include "canvas/Utilities/FriendlyName.h"

using namespace art;
namespace hepnos { 
  template <typename A, typename B>
  using Assns =std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>>; 
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
    fhicl::Atom<bool> forwardProducts{fhicl::Name("forwardProducts"), false};
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
  bool const wantForwardProducts_;
  hepnos::Run r_;
  hepnos::SubRun sr_;
  hepnos::DataStore & datastore_;
  hepnos::DataSet dataset_;
  std::map<art::ProductID, hepnos::ProductID> translator_;
}; // HepnosOutput

HepnosOutput::HepnosOutput(Parameters const& ps)
  : OutputModule{ps().omConfig, ps.get_PSet()}
  , wantForwardProducts_{ps().forwardProducts()}
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
  auto prodIds = h_e.listProducts("");
  hepnos::UUID datasetId;
  hepnos::RunNumber run;
  hepnos::SubRunNumber subrun;
  hepnos::EventNumber hepnosevent;
  std::string hepnostag;
  std::string type;
  for (auto p: prodIds) {
    p.unpackInformation(&datasetId, &run, &subrun, &hepnosevent, &hepnostag, &type);
    std::cout << "Tag: " << hepnostag << "#### Type: " << type << "\n";
    std::cout << "Uniform type: " << art::uniform_type_name(type) << "\n";
    auto [label, instance, processname] = splitTag(hepnostag);  
    auto t = art::uniform_type_name(type);
    auto const product_name = art::canonicalProductName(art::friendlyname::friendlyName(t), 
                                        label, 
                                        instance, 
                                        processname);
    auto art_pid = art::ProductID{product_name};
    if (translator_.find(art_pid) != translator_.cend()) continue;
    translator_[art_pid] = p;
    std::cout << "art/hepnos: " << art_pid << ", " << product_name << "\n";   
   }
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
    std::cout << "art: " << pd.productID() << ", " << pd.branchName() << ", " << pd.processName()<< "\n";   
    if (translator_.find(pd.productID()) != translator_.cend()) continue;
    auto const& oh = p.getForOutput(pd.productID(), true);
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
    std::cout << "art/assns: " << pd.productID() << ", " << pd.branchName() << "\n";   
    if (translator_.find(pd.productID()) != translator_.cend()) continue;
    auto const& oh = p.getForOutput(pd.productID(), true);
    
    //dynamic cast to the type we care about is needed here
    EDProduct const* product = oh.isValid() ? oh.wrapper() : nullptr;
    if (auto pwt = prodWithType<art::Assns<raw::RawDigit,recob::Wire,void>>(product, pd)) 
       storeassns(datastore_, h_e, translator_, pd.inputTag(), *pwt); 
    else if (auto pwt = prodWithType<art::Assns<recob::Wire,recob::Hit,void>>(product, pd)) { 
      storeassns(datastore_, h_e, translator_, pd.inputTag(), *pwt); 
   }
   }
}


DEFINE_ART_MODULE(HepnosOutput)
