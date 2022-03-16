#include "catch.hpp"

#include "../src/serialization/cluster_serialization.h"

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <fstream>
#include <utility>

TEST_CASE("writing a cluster works")
{
  unsigned int nhits        = 2000;
  float        wire_start   = -0.1;
  float        wire_end     = +0.1;
  float        sigmaw_start = -0.01;
  float        sigmaw_end   = +0.01;
  float        tick_start   = -100;
  float        tick_end     = +100;
  float        sigmat_start = -0.1;
  float        sigmat_end   = +0.1;
  float        charge_start = 2.0;
  float        charge_end   = 4.0;
  float        angle_start  = 10.0;
  float        angle_end    = 20.0;
  float        oangle_start = 3.0;
  float        oangle_end   = 5.0;
  float        integral     = 1000;
  float        sumadc       = 1200;
  float        int_stddev   = 10;
  float        sadc_stddev  = 12;
  float        mhit_density = 1.2;
  float        width        = 1.3;
  int          idt          = 3000;
  geo::View_t  view         = geo::View_t::k3D;
  unsigned int pid_planeid  = 111;
  unsigned int pid_tpcid    = 222;
  unsigned int pid_tpccryo  = 333;

  recob::Cluster c1(wire_start, sigmaw_start, tick_start, sigmat_start, charge_start, angle_start, oangle_start,
      wire_end, sigmaw_end, tick_end, sigmat_end, charge_end, angle_end, oangle_end,
      integral, int_stddev, sumadc, sadc_stddev, nhits, mhit_density, width, idt,
      view, static_cast<const geo::PlaneID&>(geo::PlaneID(pid_tpccryo, pid_tpcid, pid_planeid)));

  {
    std::ofstream ofs("clusterout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << c1;
    CHECK(ofs.good());
    ofs.close();
  }

  std::ifstream ifs("clusterout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Cluster c2;
  ia >> c2;
  CHECK(c1 == c2);

  wire_start = wire_start - 10;
  recob::Cluster c3(wire_start, sigmaw_start, tick_start, sigmat_start, charge_start, angle_start, oangle_start,
      wire_end, sigmaw_end, tick_end, sigmat_end, charge_end, angle_end, oangle_end,
      integral, int_stddev, sumadc, sadc_stddev, nhits, mhit_density, width, idt,
      view, static_cast<const geo::PlaneID&>(geo::PlaneID(pid_tpccryo, pid_tpcid, pid_planeid)));
  CHECK_THROWS(c1 == c3);

}
