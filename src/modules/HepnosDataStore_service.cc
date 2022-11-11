#include "HepnosDataStore.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include <chrono>
#include <functional>

namespace icaruswf {

  HepnosDataStore::HepnosDataStore(Parameters const& ps)
  {

    hepnos_exec_thread_ = std::thread([this] {
      while (true) {
        std::unique_lock<std::mutex> local_lock(this->mutex);
        if (!this->work_to_do) {
          this->cond_var.wait(local_lock);
        }
        if (this->active == true) {
          std::cout << "[HEPnOS thread status] working \n";
          this->work();
          this->work_to_do = 0;
          continue;
        } else {
          std::cout << "[HEPnOS thread status] exiting \n";
          return;
        }
      }
    });

    hepnos_exec_thread_.detach();

    std::function<void(void)> f = [&]() {
      dataStore_ =
        hepnos::DataStore::connect(ps().protocol(), ps().connection_file());
      return;
    };
    this->set_work_function(f);
    this->set_work_state();
    this->wait();
  }

  /* Let the worker thread execute work function */
  void
  HepnosDataStore::wait()
  {
    while (true) {
      if (this->work_to_do == 0) {
        break;
      } else {
        continue;
      }
    }
    return;
  };

  /* Set work state */
  void
  HepnosDataStore::set_work_state()
  {
    if (this->work_to_do == 0) {
      std::unique_lock<std::mutex> local_lock(this->mutex);
      this->work_to_do = 1;
      this->cond_var.notify_one();
    } else {
      std::cerr << "[HEPnOS worker thread] set-work-state: trying to set work "
                   "when there is already work to do!";
    }
    return;
  };

  /* Set work function */
  void
  HepnosDataStore::set_work_function(std::function<void(void)> in_work)
  {
    if (this->work_to_do == 0) {
      this->work = in_work;
    } else {
      std::cerr
        << "[HEPnOS worker thread] set-work-function trying to set work "
           "when there is already work to do!";
    }
    return;
  };

  void
  HepnosDataStore::finalize()
  {
    {
      std::function<void(void)> f = [&]() {
        this->dataStore_ = hepnos::DataStore{};
        return;
      };
      this->set_work_function(f);
      this->set_work_state();
      this->wait();
    }

    this->active = false;
    this->work_to_do = 1;
    this->cond_var.notify_one();

    return;
  }

  hepnos::DataStore&
  HepnosDataStore::getStore()
  {
    return dataStore_;
  }
} // namespace icaruswf

DEFINE_ART_SERVICE(icaruswf::HepnosDataStore)
