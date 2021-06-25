#include "art/Framework/Principal/Handle.h"
#include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Core/SharedAnalyzer.h"
#include "art/Framework/Principal/fwd.h"
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
          const char * modulelabel, 
          const char *instancename="") 
{
  art::InputTag const tag(modulelabel, "", instancename);
  auto const h = a_e.getValidHandle<T>(tag);
  return h_e.store(tag, *h);
} 

template<typename A, typename B>
void
storeassns(hepnos::DataStore datastore, 
           hepnos::ProductID& A_id, 
           hepnos::ProductID& B_id, 
           hepnos::Event & h_e, 
           art::Event const& a_e, 
           const char * modulelabel, 
           const char *instancename="") 
{
  art::InputTag const assns_tag(modulelabel, "", instancename);
  auto const & a_assns = *a_e.getValidHandle<art::Assns<A, B>>(assns_tag);
  hepnos::Assns<A, B> h_assns;
  h_assns.reserve(a_assns.size());

  for (auto const & a: a_assns) {
    //std::cout << a.first.key() << "," << a.second.key() << "\n";
    auto const A_ptr = datastore.makePtr<A>(A_id, a.first.key());
    auto const B_ptr = datastore.makePtr<B>(B_id, a.second.key());
    h_assns.push_back(make_pair(A_ptr, B_ptr)); 
  }
    h_e.store(assns_tag, h_assns); 
}


namespace {
  class HepnosStoreData : public art::SharedAnalyzer {
  public:
    struct Config {
    };
  
    using Parameters = Table<Config>;
  
    explicit HepnosStoreData(Parameters const& p, art::ProcessingFrame const&)
      : SharedAnalyzer{p}
    {
      auto connection_file = "client.yaml";
      auto ds_name = "icarus";
      datastore_ = hepnos::DataStore::connect(connection_file);
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
      auto raw_id = storedata<std::vector<raw::RawDigit>>(h_e, a_e,"rawdigitfilter");
      auto w_id_1 = storedata<std::vector<recob::Wire>>(h_e, a_e, "recowireraw");
      auto w_id_2 = storedata<std::vector<recob::Wire>>(h_e, a_e, "decon1droi");
      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id, w_id_1, h_e, a_e, "recowireraw");
      storeassns<raw::RawDigit, recob::Wire>(datastore_, raw_id, w_id_2, h_e, a_e, "decon1droi");
      
      auto hit_id_1 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushitall");
      auto hit_id_2 = storedata<std::vector<recob::Hit>>(h_e, a_e, "gaushit");
      auto hit_id_3 = storedata<std::vector<recob::Hit>>(h_e, a_e, "icarushit");
      
      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_1, w_id_1, h_e, a_e, "gaushitall");
      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_2, w_id_1, h_e, a_e, "icarushit");
      storeassns<recob::Hit, recob::Wire>(datastore_, hit_id_3, w_id_1, h_e, a_e, "gaushit");
     
    }

  };
}

DEFINE_ART_MODULE(HepnosStoreData)
