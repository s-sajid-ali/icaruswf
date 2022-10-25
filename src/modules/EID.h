#if !defined(eid_h)
#define eid_h

#include <hepnos.hpp>

struct EID {
  hepnos::RunNumber run = 0;
  hepnos::SubRunNumber subrun = 0;
  hepnos::EventNumber event = 0;
};

#endif
