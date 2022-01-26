#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/fwd.h"
#include "art/Framework/Principal/Event.h"
#include "canvas/Persistency/Common/Assns.h"

#include "fhiclcpp/types/Atom.h"

#include "larcoreobj/SimpleTypesAndConstants/RawTypes.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

#include "lardataobj/RawData/RawDigit.h"
#include "lardataobj/RecoBase/Hit.h"
#include "lardataobj/RecoBase/SpacePoint.h"
#include "lardataobj/RecoBase/Wire.h"

#include "hepnos.hpp"

#include "hit_serialization.h"
#include "rawdigit_serialization.h"
#include "spacepoint_serialization.h"
#include "wire_serialization.h"

#include <boost/serialization/utility.hpp>


namespace hepnos { 
  template <typename A, typename B>
  using Assns =std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>>; 
}

template<typename T> 
auto
storedata(hepnos::Event& h_e, 
          art::Event const& a_e, 
          const char* modulelabel, 
          const char* instancename, 
          const char* processname="") 
{
  art::InputTag const tag(modulelabel, instancename, processname);
  auto const h = a_e.getValidHandle<T>(tag);
  auto d = *h;
  std::cout << modulelabel << ", " << d.size() << "\n";
  auto g = h_e.store(tag, d);
  return g;
} 

template<typename A, typename B>
void
storeassns(hepnos::DataStore datastore, 
           hepnos::ProductID& A_id, 
           hepnos::ProductID& B_id, 
           hepnos::Event & h_e, 
           art::Event const& a_e, 
           const char* modulelabel, 
           const char* instancename, 
           const char* processname="") 
{
  art::InputTag const assns_tag(modulelabel, instancename, processname);
  auto const & a_assns = *a_e.getValidHandle<art::Assns<A, B>>(assns_tag);
  hepnos::Assns<A, B> h_assns;
  h_assns.reserve(a_assns.size());

  for (auto const & a: a_assns) {
    //std::cout << a.first.key() << "," << a.second.key() << "\n";
    auto const A_ptr = datastore.makePtr<A>(A_id, a.first.key());
    auto const B_ptr = datastore.makePtr<B>(B_id, a.second.key());
    h_assns.push_back(std::make_pair(A_ptr, B_ptr)); 
  }
    h_e.store(assns_tag, h_assns); 
}


namespace {
  class HepnosStoreDataNew : public art::SharedAnalyzer { public:
  public:
    struct Config {
    };
  
    using Parameters = Table<Config>;
  
    explicit HepnosStoreDataNew(Parameters const& p, art::ProcessingFrame const&)
      : SharedAnalyzer{p}
    {
      async<art::InEvent>();
      auto connection_file = "connection.json";
      auto ds_name = "icarus";
      datastore_ = hepnos::DataStore::connect("ofi+tcp", connection_file);
      hepnos::DataSet current = datastore_.root();
      current = current.createDataSet(ds_name); 
      auto r = current.createRun(1000); 
      sr_ = r.createSubRun(34);
   }

  private:
    hepnos::SubRun sr_;
    hepnos::DataStore datastore_;

    void
    analyze(art::Event const& a_e , 
            art::ProcessingFrame const&) override
    {
      std::cout << "Processing events " << a_e.event() << '\n'; 
      hepnos::Event h_e = sr_.createEvent(a_e.event()); 
      auto raw_id_0 = storedata<std::vector<raw::RawDigit>>(h_e, a_e,"daq0", "PHYSCRATEDATATPCEE");
      auto raw_id_1 = storedata<std::vector<raw::RawDigit>>(h_e, a_e,"daq1", "PHYSCRATEDATATPCEW");
      auto raw_id_2 = storedata<std::vector<raw::RawDigit>>(h_e, a_e,"daq2", "PHYSCRATEDATATPCWE");
      auto raw_id_3 = storedata<std::vector<raw::RawDigit>>(h_e, a_e,"daq3", "PHYSCRATEDATATPCWW");
//      auto w_id_0 = storedata<std::vector<recob::Wire>>(h_e, a_e, "decon1DroiTPC0");
//      auto w_id_1 = storedata<std::vector<recob::Wire>>(h_e, a_e, "decon1DroiTPC1");
//      auto w_id_2 = storedata<std::vector<recob::Wire>>(h_e, a_e, "decon1DroiTPC2");
//      auto w_id_3 = storedata<std::vector<recob::Wire>>(h_e, a_e, "decon1DroiTPC3");
//      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id_0, w_id_0, h_e, a_e, "decon1DroiTPC0");
//      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id_1, w_id_1, h_e, a_e, "decon1DroiTPC1");
//      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id_2, w_id_2, h_e, a_e, "decon1DroiTPC2");
//      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id_3, w_id_3, h_e, a_e, "decon1DroiTPC3");
//      
//      auto hit_id_0 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushitTPC0");
//      auto hit_id_1 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushitTPC1");
//      auto hit_id_2 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushitTPC2");
//      auto hit_id_3 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushitTPC3");
//    //  auto hit_id_2 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushit");
//    //  auto hit_id_3 = storedata<std::vector<recob::Hit>>(h_e, a_e, "icarushit");
//      storeassns<raw::RawDigit, recob::Hit>(datastore_, raw_id_0, hit_id_0, h_e, a_e, "gaushitTPC0");
//      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_0, w_id_0, h_e, a_e, "gaushitTPC0");
//      storeassns<raw::RawDigit, recob::Hit>(datastore_, raw_id_1, hit_id_1, h_e, a_e, "gaushitTPC1");
//      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_1, w_id_1, h_e, a_e, "gaushitTPC1");
//      storeassns<raw::RawDigit, recob::Hit>(datastore_, raw_id_2, hit_id_2, h_e, a_e, "gaushitTPC2");
//      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_2, w_id_2, h_e, a_e, "gaushitTPC2");
//      storeassns<raw::RawDigit, recob::Hit>(datastore_, raw_id_3, hit_id_3, h_e, a_e, "gaushitTPC3");
//      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_3, w_id_3, h_e, a_e, "gaushitTPC3");
//     
//      storedata<std::vector<recob::SpacePoint>>(h_e, a_e, "pandoraGaus");
      }
   };

}

DEFINE_ART_MODULE(HepnosStoreDataNew)
