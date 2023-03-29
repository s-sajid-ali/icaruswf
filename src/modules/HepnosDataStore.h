#ifndef HEPNOSDATASTORE_H
#define HEPNOSDATASTORE_H

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "art/Framework/Services/Registry/ServiceTable.h"
#include "fhiclcpp/types/Atom.h"
#include "hepnos.hpp"

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

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
    explicit HepnosDataStore(Parameters const& ps);
    hepnos::DataStore& getStore();

    /* Let the worker thread execute work function */
    void wait();

    /* Set work state */
    void set_work_state();

    /* Set work function */
    void set_work_function(std::function<void(void)> in_work);

    /* Cleanup, reset the dataStore_ instruct the worker thread to stop */
    void finalize();

    /* Destructor resets the dataStore_
     and asks the worker thread to stop */
    ~HepnosDataStore()
    {
      this->finalize();
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

    /* Mutex */
    std::mutex mutex_hepnos_thread;
    std::mutex mutex_calling_thread_resume;

    /* Condition Variable to singal work status */
    std::condition_variable cond_var_work_status;
    std::condition_variable cond_var_resume_execution;
  };
} // namespace icaruswf

DECLARE_ART_SERVICE(icaruswf::HepnosDataStore, SHARED)

#endif
