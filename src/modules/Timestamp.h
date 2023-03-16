#ifndef HEPNOS_TIMESTAMP_H
#define HEPNOS_TIMESTAMP_H

#include <chrono>
#include <fstream>
#include <vector>

#include "art/Framework/Services/Registry/ServiceDeclarationMacros.h"
#include "canvas/Persistency/Provenance/EventID.h"
#include "fhiclcpp/ParameterSet.h"

namespace art {
  class ScheduleContext;
  class ModuleContext;
  class ActivityRegistry;
  class Event;
}

namespace icaruswf {

  using time_point = std::chrono::time_point<std::chrono::system_clock>;

  struct Record {
    time_point when;
    unsigned int cpu;
    unsigned int numa_node;
  };

  std::ostream& operator<<(std::ostream& os, Record const& r);

  struct TimestampData {
    art::EventID eid;
    Record start_read;
    Record end_read;
    Record start_write;
    Record end_write;
  };

  std::ostream& operator<<(std::ostream& os, TimestampData const& d);

  class Timestamp {
  private:
    std::vector<TimestampData> data_;
    std::ofstream file_;

  public:
    Timestamp(fhicl::ParameterSet const&, art::ActivityRegistry& ar);
    ~Timestamp();
    void startOfRead(art::ScheduleContext);
    void endOfRead(art::Event const& e, art::ScheduleContext);
    void updateEndOfRead();
    void startOfWrite(art::ModuleContext const&);
    void endOfWrite(art::ModuleContext const&);
  };

}

DECLARE_ART_SERVICE(icaruswf::Timestamp, SHARED)
#endif
