
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

//#include "lardataobj/RawData/RawDigit.h"
//#include "lardataobj/RecoBase/Hit.h"
//#include "lardataobj/RecoBase/SpacePoint.h"
//#include "lardataobj/RecoBase/Wire.h"

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
  
  template <typename T>
  std::unique_ptr<std::vector<T>> 
  read_product(hepnos::Event& event,
              std::string const& module_label,
              bool strict) 
  {
     art::InputTag const tag(module_label, "", "");
     std::vector<T> products;
     auto ret = event.load(tag, products);
     if (ret) return std::make_unique<std::vector<T>>(products);
     if (strict) 
     { 
        throw art::Exception(art::errors::ProductNotFound) <<  "Product not read for label: "<< module_label << ", " << art::TypeID{products}.friendlyClassName() << "\n"; 
     }
     return {};
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
  read_assns(hepnos::Event& event, 
            art::EventPrincipal*& outE, 
            std::vector<A> const& a_col, 
            std::vector<B> const& b_col, 
            std::string const& a_module_label,
            std::string const& b_module_label)
  {
     art::InputTag const tag(a_module_label, "", "");
     std::vector<std::pair<hepnos::Ptr<A>, hepnos::Ptr<B>>> assns;
     event.load(tag, assns);
     auto const a_pid = create_productID<A>(a_col, a_module_label, "");
     auto const b_pid = create_productID<B>(b_col, b_module_label, "");
     art::Assns<A, B> art_assns;
     for (auto const& [a, b]: assns) {
       art_assns.addSingle(art::Ptr<A>(a_pid, a.m_index, outE->productGetter(a_pid)), 
                           art::Ptr<B>(b_pid, b.m_index, outE->productGetter(b_pid)));
     }
     return std::make_unique<art::Assns<A, B>>(art_assns);
  }
  

  // Funtion in hepnos namespace to read in all the data products and association
  // collection needed in this workflow. We are working with data products of 
  // known types here, and hence we have a sequence of similar steps to read 
  // in each product one by one. We may want to factorize this code a little better. 
  bool
  read_all(hepnos::Event& event, 
          bool strict, 
          art::EventPrincipal*& outE) 
  {

      auto rawdigits = hepnos::read_product<raw::RawDigit>(event, "rawdigitfilter", strict);
      if (rawdigits) art::put_product_in_principal(std::move(rawdigits), *outE, "rawdigitfilter");
      
      auto wires = hepnos::read_product<recob::Wire>(event, "recowireraw", strict);
      if (wires) {
        art::put_product_in_principal(std::move(wires), *outE, "recowireraw");
      }

      if (rawdigits && wires) {
        auto assns = hepnos::read_assns<raw::RawDigit, recob::Wire>(event, outE, *rawdigits, *wires, "rawdigitfilter", "recowireraw");
        art::put_product_in_principal(std::move(assns), *outE, "recowireraw"); 
      }
       
      auto wires_dcon = hepnos::read_product<recob::Wire>(event, "dcon1droi", strict);
      if (wires_dcon) {
        art::put_product_in_principal(std::move(wires_dcon), *outE, "dcon1droi");
      }

      if (rawdigits && wires_dcon) {
        auto assns = hepnos::read_assns<raw::RawDigit, recob::Wire>(event, outE, *rawdigits, *wires_dcon, "rawdigitfilter","dcon1droi");
        art::put_product_in_principal(std::move(assns), *outE, "dcon1droi"); 
      }
      
      auto hits = hepnos::read_product<recob::Hit>(event, "gaushitall", strict);
      if (hits) art::put_product_in_principal(std::move(hits), *outE, "gaushitall");
      
      if (hits && wires) {
        auto assns = hepnos::read_assns<recob::Hit, recob::Wire>(event, outE, *hits, *wires, "gaushitall","recowireraw");
        art::put_product_in_principal(std::move(assns), *outE, "gaushitall");
      }

      auto gaushits = hepnos::read_product<recob::Hit>(event, "gaushit", strict);
      if (gaushits) art::put_product_in_principal(std::move(gaushits), *outE, "gaushit");
      
      if (gaushits && wires) {
        auto assns = hepnos::read_assns<recob::Hit, recob::Wire>(event, outE, *gaushits, *wires, "gaushit","recowireraw");
        art::put_product_in_principal(std::move(assns), *outE, "gaushit");
      }

      auto icarushits = hepnos::read_product<recob::Hit>(event, "icarushit", strict);
      if (icarushits) art::put_product_in_principal(std::move(icarushits), *outE, "icarushit");
      
      if (icarushits && wires) {
        auto assns = hepnos::read_assns<recob::Hit, recob::Wire>(event, outE, *icarushits, *wires, "icarushit","recowireraw");
        art::put_product_in_principal(std::move(assns), *outE, "icarushit");
      }

      auto spacepoints_1 = hepnos::read_product<recob::SpacePoint>(event, "pandoraGaus", strict);
      if (spacepoints_1) art::put_product_in_principal(std::move(spacepoints_1), *outE, "pandoraGaus");

      auto spacepoints_2 = hepnos::read_product<recob::SpacePoint>(event, "pandoraICARUS", strict);
      if (spacepoints_2) art::put_product_in_principal(std::move(spacepoints_2), *outE, "pandoraICARUS");

      auto spacepoints_3 = hepnos::read_product<recob::SpacePoint>(event, "pandoraKalmanTrackICARUS", strict);
      if (spacepoints_3) art::put_product_in_principal(std::move(spacepoints_3), *outE, "pandoraKalmanTrackICARUS");

      auto spacepoints_4 = hepnos::read_product<recob::SpacePoint>(event, "pandoraKalmanTrackGaus", strict);
      if (spacepoints_4) art::put_product_in_principal(std::move(spacepoints_4), *outE, "pandoraKalmanTrackGaus");

  return true;
  }
}

namespace hepnossource {
  class HepnosInputSource {
  public:
    HepnosInputSource(fhicl::ParameterSet const& p, 
                      art::ProductRegistryHelper& rh, 
                      art::SourceHelper const& pm) : pm_(pm)
    {
      rh.reconstitutes<std::vector<raw::RawDigit>, art::InEvent>("rawdigitfilter");
      rh.reconstitutes<std::vector<recob::Wire>, art::InEvent>("recowireraw");
      rh.reconstitutes<std::vector<recob::Wire>, art::InEvent>("decon1droi");
      rh.reconstitutes<art::Assns<raw::RawDigit, recob::Wire>, art::InEvent>("recowireraw");
      rh.reconstitutes<art::Assns<raw::RawDigit, recob::Wire>, art::InEvent>("decon1droi");
      rh.reconstitutes<std::vector<recob::Hit>, art::InEvent>("gaushitall");
      rh.reconstitutes<std::vector<recob::Hit>, art::InEvent>("gaushit");
      rh.reconstitutes<std::vector<recob::Hit>, art::InEvent>("icarushit");
      rh.reconstitutes<art::Assns<recob::Hit, recob::Wire>, art::InEvent>("gaushitall");
      rh.reconstitutes<art::Assns<recob::Hit, recob::Wire>, art::InEvent>("icarushit");
      rh.reconstitutes<art::Assns<recob::Hit, recob::Wire>, art::InEvent>("gaushit");
      rh.reconstitutes<std::vector<recob::SpacePoint>, art::InEvent>("pandoraGaus");
      rh.reconstitutes<std::vector<recob::SpacePoint>, art::InEvent>("pandoraICARUS");
      rh.reconstitutes<std::vector<recob::SpacePoint>, art::InEvent>("pandoaKalmanTrackICARUS");
      rh.reconstitutes<std::vector<recob::SpacePoint>, art::InEvent>("pandoraKalmanTrackGaus");
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
      
      auto const strict = true; 
      auto status = hepnos::read_all(*ev_, strict, outE);     
      ++ev_;
      return status;
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
