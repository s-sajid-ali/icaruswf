#include "Timestamp.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Services/Registry/ServiceDefinitionMacros.h"
#include "art/Framework/Services/Registry/ActivityRegistry.h"

#include <numa.h>
#include <sched.h>

std::ostream&
icaruswf::operator<<(std::ostream& os, icaruswf::Record const& r)
{
  os << r.when.time_since_epoch().count() << '\t' << r.cpu << '\t'
     << r.numa_node;
  return os;
}

std::ostream&
icaruswf::operator<<(std::ostream& os, icaruswf::TimestampData const& d)
{
  os << d.eid.run() << '\t' << d.eid.subRun() << '\t' << d.eid.event() << '\t'
     << d.start_read << '\t' << d.end_read << '\t' << d.start_write << '\t'
     << d.end_write;
  return os;
}

icaruswf::Timestamp::Timestamp(fhicl::ParameterSet const&,
                               art::ActivityRegistry& ar)
  : file_("io_timestamps.tsv")
{
  ar.sPreSourceEvent.watch(this, &icaruswf::Timestamp::startOfRead);
  ar.sPostSourceEvent.watch(this, &icaruswf::Timestamp::endOfRead);
  ar.sPreWriteEvent.watch(this, &icaruswf::Timestamp::startOfWrite);
  ar.sPostWriteEvent.watch(this, &icaruswf::Timestamp::endOfWrite);
  data_.reserve(1000);
  file_ << "run\tsubrun\tevent\tsrtime\tsrcpu\tsrnode\tertime\tercpu\ternode\ts"
           "wtime\tswcpu\tswnode\tewtime\tewcpu\tewnode\n";
}

icaruswf::Timestamp::~Timestamp()
{
  for (auto const& d : data_) {
    file_ << d << "\n";
  }
}

icaruswf::Record
makeRecord()
{
  unsigned int cpu = sched_getcpu();
  unsigned int node = numa_node_of_cpu(cpu);
  return {std::chrono::system_clock::now(), cpu, node};
}

void 
icaruswf::Timestamp::startOfRead(art::ScheduleContext)
{
  data_.emplace_back();
  data_.back().start_read = makeRecord();
}

void
icaruswf::Timestamp::endOfRead(art::Event const& e, art::ScheduleContext)
{
  data_.back().eid = e.id();
  data_.back().end_read = makeRecord();
}

void
icaruswf::Timestamp::updateEndOfRead()
{
  data_.back().end_read = makeRecord();
}

void
icaruswf::Timestamp::startOfWrite(art::ModuleContext const&)
{
  data_.back().start_write = makeRecord();
}

void
icaruswf::Timestamp::endOfWrite(art::ModuleContext const&)
{
  data_.back().end_write = makeRecord();
}

DEFINE_ART_SERVICE(icaruswf::Timestamp)
