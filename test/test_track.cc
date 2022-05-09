#include "catch.hpp"

#include "../src/serialization/track_serialization.h"

#include <larcoreobj/SimpleTypesAndConstants/RawTypes.h>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/version.hpp>

#include <fstream>
#include <random>

TEST_CASE("writing an track works")
{
  int   pid  = 1;
  float chi2 = 1.1;
  int   ndof = 2;
  int   id   = 3;

  std::array<Double32_t, 15> fcvtx_arr;
  std::array<Double32_t, 15> fcend_arr;
  std::vector<recob::TrajectoryPointFlags::HitIndex_t> tracktraj_flaghitindices;
  std::vector<std::array<bool, tpf_maxflags>> tracktraj_flagstates;
  std::vector<std::array<root_pvscalar_type,3>> traj_pos;
  std::vector<std::array<root_dvscalar_type,3>> traj_momenta;

  size_t traj_size = 10;
  bool traj_hasmomenta = true;

  traj_pos.resize(traj_size);
  traj_momenta.resize(traj_size);
  tracktraj_flaghitindices.resize(traj_size);
  tracktraj_flagstates.resize(traj_size);

  /* Populate the vectors! */
  {
    std::random_device rd{};
    std::mt19937_64 gen{rd()};

    std::uniform_real_distribution<Double32_t> dist1{std::numeric_limits<Double32_t>::min(), std::numeric_limits<Double32_t>::max()};
    std::generate(fcvtx_arr.begin(), fcvtx_arr.end(), [&dist1, &gen](){ return dist1(gen); });
    std::generate(fcend_arr.begin(), fcend_arr.end(), [&dist1, &gen](){ return dist1(gen); });

    std::uniform_int_distribution<recob::TrajectoryPointFlags::HitIndex_t>
      dist2{std::numeric_limits<recob::TrajectoryPointFlags::HitIndex_t>::min(),
        std::numeric_limits<recob::TrajectoryPointFlags::HitIndex_t>::max()};
    std::generate(tracktraj_flaghitindices.begin(), tracktraj_flaghitindices.end(), [&dist2, &gen](){ return dist2(gen); });
    std::uniform_int_distribution<int> dist3{std::numeric_limits<int>::min(), std::numeric_limits<int>::max()};
    std::for_each(tracktraj_flagstates.begin(), tracktraj_flagstates.end(),
        [&](auto x){ std::generate(x.begin(), x.end(), [&dist3, &gen](){ return (dist3(gen) > 0) ? true : false; } ); });

    /* temporary fix if no flag is active */
    std::fill(tracktraj_flagstates[0].begin(), tracktraj_flagstates[0].end(), true);

    std::uniform_real_distribution<root_pvscalar_type>
      dist4{std::numeric_limits<root_pvscalar_type>::min(), std::numeric_limits<root_pvscalar_type>::max()};
    std::for_each(traj_pos.begin(), traj_pos.end(),
        [&](auto x){ std::generate(x.begin(), x.end(), [&dist4, &gen] {return dist4(gen);});});

    std::uniform_real_distribution<root_dvscalar_type>
      dist5{std::numeric_limits<root_dvscalar_type>::min(), std::numeric_limits<root_dvscalar_type>::max()};
    std::for_each(traj_momenta.begin(), traj_momenta.end(),
        [&](auto x){ std::generate(x.begin(), x.end(), [&dist5, &gen] {return dist5(gen);});});
  }

  /* generate the tpf vector from hits and flags! */
  std::vector<recob::TrajectoryPointFlags> tracktraj_flags;
  tracktraj_flags.resize(traj_size);
  std::transform(tracktraj_flaghitindices.begin(), tracktraj_flaghitindices.end(),
      tracktraj_flagstates.begin(),
      tracktraj_flags.begin(),
      [&](auto x, auto y){
      tpf_flagset_type flagset;
      for(tpf_flagindex_type tpf_flag_idx=0; tpf_flag_idx<tpf_maxflags; tpf_flag_idx++){
      auto idx = static_cast<size_t>(tpf_flag_idx); if (y[idx]){flagset.set(tpf_flag_idx);}}
      return recob::TrajectoryPointFlags(x, flagset.mask());
      }
      );

  recob::Track t1(
      recob::TrackTrajectory(
        std::move(recob::tracking::convertCollToPoint(traj_pos)),
        std::move(recob::tracking::convertCollToVector(traj_momenta)),
        std::move(tracktraj_flags),
        traj_hasmomenta),
      pid, chi2, ndof,
      recob::tracking::SMatrixSym55(fcvtx_arr.begin(), fcvtx_arr.end()),
      recob::tracking::SMatrixSym55(fcend_arr.begin(), fcend_arr.end()),
      id);

  {
    std::ofstream ofs("trackout", std::ios::binary);
    boost::archive::binary_oarchive oa(ofs);
    oa << t1;
    CHECK(ofs.good());
    ofs.close();
  }


  std::ifstream ifs("trackout", std::ios::binary);
  CHECK(ifs.good());
  boost::archive::binary_iarchive ia(ifs);
  recob::Track t2;
  ia >> t2;

  CHECK(t1 == t2);

  /* re-create a new tracktraj_flags vector as it has been moved */
  std::vector<recob::TrajectoryPointFlags> tracktraj_flags_new;
  tracktraj_flags_new.resize(traj_size);

  /* clear the flags */
  std::transform(tracktraj_flaghitindices.begin(), tracktraj_flaghitindices.end(),
      tracktraj_flags_new.begin(),
      [&](auto x){
      tpf_flagset_type flagset;
      flagset.clear();
      return recob::TrajectoryPointFlags(x, flagset.mask());
      }
      );

  /* create another track with cleared flags */
  recob::Track t3(
      recob::TrackTrajectory(
        std::move(recob::tracking::convertCollToPoint(traj_pos)),
        std::move(recob::tracking::convertCollToVector(traj_momenta)),
        std::move(tracktraj_flags_new),
        traj_hasmomenta),
      pid, chi2, ndof,
      recob::tracking::SMatrixSym55(fcvtx_arr.begin(), fcvtx_arr.end()),
      recob::tracking::SMatrixSym55(fcend_arr.begin(), fcend_arr.end()),
      id);

  CHECK_THROWS(t1==t3);
}
