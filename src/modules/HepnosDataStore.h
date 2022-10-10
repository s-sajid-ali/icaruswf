#ifndef HEPNOSDATASTORE_H
#define HEPNOSDATASTORE_H

#include <art/Framework/Services/Registry/ServiceDeclarationMacros.h>
#include <art/Framework/Services/Registry/ServiceTable.h>
#include <fhiclcpp/types/Atom.h>
#include <hepnos.hpp>

#include <atomic>
#include <thread>
#include <memory>

#include "icaruswf_config.h"

namespace icaruswf {
  class HepnosDataStore {
    public:
      struct Config {
        fhicl::Atom<std::string> protocol{fhicl::Name("protocol"),
          mercury_transport_protocol};
        fhicl::Atom<std::string> connection_file{fhicl::Name("connection_file"),
          "connection.json"};
      };
      using Parameters = art::ServiceTable<Config>;
      explicit HepnosDataStore(Parameters const &ps);
      hepnos::DataStore &getStore();

      /* Let the worker thread execute work function */
      void wait();

      /* Set work state */
      void set_work_state();

      /* Set work function */
      void set_work_function(std::function<void(void)> in_work);

      /* Destructor asks worker thread to stop */
      ~HepnosDataStore(){
        {
          std::function<void(void)> f = [&]() {
            //delete &(this->dataStore_);
            std::destroy_at(&(this->dataStore_));
            return;
          };
          this->set_work_function(f);
          this->set_work_state();
          this->wait();
        }
        this->active = false;

        std::destroy_at(&active);
        std::destroy_at(&work_to_do);
        std::destroy_at(&work);

        return;

      }

    private:
      hepnos::DataStore dataStore_;

      /* Detached thread which will execute work */
      std::thread hepnos_exec_thread_;

      /* is the worker thread active? */
      bool active = true;

      /* Set flag to 1 when there is work to do */
      std::atomic<int> work_to_do = 0;

      /* Work to be done */
      std::function<void(void)> work;

  };
} // namespace icaruswf

DECLARE_ART_SERVICE(icaruswf::HepnosDataStore, SHARED)

#endif
