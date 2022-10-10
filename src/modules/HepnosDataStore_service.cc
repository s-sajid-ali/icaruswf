#include "HepnosDataStore.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include <chrono>
#include <functional>

namespace icaruswf {

HepnosDataStore::HepnosDataStore(Parameters const &ps) {

  hepnos_exec_thread_ = std::thread([this] {
    while (this->active == true) {
      if (this->work_to_do) {
        std::cout << "[HEPnOS thread status] working \n";
        work();
        this->work_to_do = 0;
      } else {
        // std::cout << "[HEPnOS thread status] going back to sleep \n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
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
void HepnosDataStore::wait() {
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
void HepnosDataStore::set_work_state() {
  if (this->work_to_do == 0) {
    this->work_to_do = 1;
  } else {
    std::cerr << "[HEPnOS worker thread] set-work-state: trying to set work "
                 "when there is already work to do!";
  }
  return;
};

/* Set work function */
void HepnosDataStore::set_work_function(std::function<void(void)> in_work) {
  if (this->work_to_do == 0) {
    this->work = in_work;
  } else {
    std::cerr << "[HEPnOS worker thread] set-work-function trying to set work "
                 "when there is already work to do!";
  }
  return;
};

hepnos::DataStore &HepnosDataStore::getStore() { return dataStore_; }
} // namespace icaruswf

DEFINE_ART_SERVICE(icaruswf::HepnosDataStore)
