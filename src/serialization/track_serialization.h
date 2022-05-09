#if !defined(track_serialization_h)
#define track_serialization_h

#include <iostream>
#include <map>

#include <lardataobj/RecoBase/Track.h>
#include <lardataobj/RecoBase/TrackingTypes.h>
#include <lardataobj/RecoBase/TrackTrajectory.h>
#include <lardataobj/RecoBase/TrajectoryPointFlags.h>
#include <lardataobj/Utilities/FlagSet.h>

#include <boost/serialization/serialization.hpp>
#include <boost/serialization/split_free.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/array.hpp>

#include "utils.h"

/* as of lardataobj-v09_06_03, maxFlags is 32, flagindex_type is unsigned int */
constexpr auto tpf_maxflags = recob::TrajectoryPointFlagTraits::maxFlags();
using tpf_flagset_type = util::flags::FlagSet<tpf_maxflags>;
using tpf_flagindex_type = tpf_flagset_type::FlagIndex_t;
using tpf_flag_type = tpf_flagset_type::Flag_t;

namespace recob {

  inline bool operator==(const recob::Track &t1, const recob::Track &t2) {
    if (t1.ParticleId() != t2.ParticleId()) {
      throw std::runtime_error("particle id values are not same");
      return false;}
    if (t1.Chi2() != t2.Chi2()) {
      throw std::runtime_error("chi2 values are not same");
      return false;}
    if (t1.Ndof() != t2.Ndof()) {
      throw std::runtime_error("ndof values are not same");
      return false;}
    if (t1.ID() != t2.ID()) {
      throw std::runtime_error("track id values are not same");
      return false;}

    const recob::TrackTrajectory& tracktraj1 = t1.Trajectory();
    const recob::TrackTrajectory& tracktraj2 = t2.Trajectory();

    const recob::Trajectory& traj1 = tracktraj1.Trajectory();
    const recob::Trajectory& traj2 = tracktraj2.Trajectory();

    if (tracktraj1.HasMomentum() != (tracktraj2.HasMomentum())) {
      throw std::runtime_error("underlying has_momenta flags for trajectories are not same");
      return false;}
    if (tracktraj1.NPoints() != tracktraj2.NPoints()) {
      throw std::runtime_error("number of points in underlying trajectories are not same");
      return false;}
    /* Now that we have verified that both trajectories have the same number of
       points above, we store the size and re-use for other checks */
    size_t traj_size = t1.Trajectory().Trajectory().NPoints();


    std::array<Double32_t, 15> fcvtx_arr1, fcvtx_arr2;
    std::copy(t1.VertexCovarianceLocal5D().begin(), t1.VertexCovarianceLocal5D().end(),
        fcvtx_arr1.data());
    std::copy(t2.VertexCovarianceLocal5D().begin(), t2.VertexCovarianceLocal5D().end(),
        fcvtx_arr2.data());
    if (compare(fcvtx_arr1, fcvtx_arr2, fcvtx_arr1.size()) == false) {
      throw std::runtime_error("fcvtx_arr components are different!");
      return false;}

    std::array<Double32_t, 15> fcend_arr1, fcend_arr2;
    std::copy(t1.EndCovarianceLocal5D().begin(), t1.EndCovarianceLocal5D().end(),
        fcend_arr1.data());
    std::copy(t2.EndCovarianceLocal5D().begin(), t2.EndCovarianceLocal5D().end(),
        fcend_arr2.data());
    if (compare(fcend_arr1, fcend_arr2, fcend_arr1.size()) == false) {
      throw std::runtime_error("fcend_arr components are different!");
      return false;}

    std::vector<std::array<root_pvscalar_type,3>> traj_pos1, traj_pos2;
    traj_pos1.resize(traj_size);
    traj_pos2.resize(traj_size);
    std::for_each(traj_pos1.begin(), traj_pos1.end(),
        [&](auto& x) { (traj1.LocationAtPoint(&x - &traj_pos1[0])).GetCoordinates(x.begin(), x.end()); } );
    std::for_each(traj_pos2.begin(), traj_pos2.end(),
        [&](auto& x) { (traj2.LocationAtPoint(&x - &traj_pos2[0])).GetCoordinates(x.begin(), x.end()); } );
    if (compare(traj_pos1, traj_pos2, traj_pos1.size()) == false) {
      throw std::runtime_error("traj_pos components are different!");
      return false;}

    std::vector<std::array<root_dvscalar_type,3>> traj_momenta1, traj_momenta2;
    traj_momenta1.resize(traj_size);
    traj_momenta2.resize(traj_size);
    std::for_each(traj_momenta1.begin(), traj_momenta1.end(),
        [&](auto& x) { (traj1.MomentumVectorAtPoint(&x - &traj_momenta1[0])).GetCoordinates(x.begin(), x.end()); } );
    std::for_each(traj_momenta2.begin(), traj_momenta2.end(),
        [&](auto& x) { (traj2.MomentumVectorAtPoint(&x - &traj_momenta2[0])).GetCoordinates(x.begin(), x.end()); } );
    if (compare(traj_momenta1, traj_momenta2, traj_momenta1.size()) == false) {
      throw std::runtime_error("traj_momenta components are different!");
      return false;}

    std::vector<recob::TrajectoryPointFlags> tracktraj_flags1 = t1.Trajectory().Flags();
    std::vector<recob::TrajectoryPointFlags> tracktraj_flags2 = t2.Trajectory().Flags();
    if (compare(tracktraj_flags1, tracktraj_flags2, tracktraj_flags1.size()) == false) {
      throw std::runtime_error("tracktraj_flags components are different!");
      return false;}

    return true;
  }
} // namespace recob

namespace boost {
  namespace serialization {
    template <class Archive>
      void save(Archive &ar, const recob::Track &t, const unsigned int) {
        int pid = t.ParticleId();
        float chi2 = t.Chi2();
        int ndof = t.Ndof();
        int id = t.ID();

        std::array<Double32_t, 15> fcvtx_arr;
        std::copy(t.VertexCovarianceLocal5D().begin(), t.VertexCovarianceLocal5D().end(),
            fcvtx_arr.data());

        std::array<Double32_t, 15> fcend_arr;
        std::copy(t.EndCovarianceLocal5D().begin(), t.EndCovarianceLocal5D().end(),
            fcend_arr.data());

        const recob::TrackTrajectory& tracktraj = t.Trajectory();
        const recob::Trajectory& traj = tracktraj.Trajectory();
        const size_t traj_size = traj.NPoints();

        std::vector<std::array<root_pvscalar_type,3>> traj_pos;
        std::vector<std::array<root_dvscalar_type,3>> traj_momenta;

        traj_pos.resize(traj_size);
        traj_momenta.resize(traj_size);

        std::for_each(traj_pos.begin(), traj_pos.end(),
            [&](auto& x) { (traj.LocationAtPoint(&x - &traj_pos[0])).GetCoordinates(x.begin(), x.end()); } );
        std::for_each(traj_momenta.begin(), traj_momenta.end(),
            [&](auto& x) { (traj.MomentumVectorAtPoint(&x - &traj_momenta[0])).GetCoordinates(x.begin(), x.end()); } );

        const bool traj_hasmomenta = traj.HasMomentum();

        std::vector<recob::TrajectoryPointFlags::HitIndex_t> tracktraj_flaghitindices;
        tracktraj_flaghitindices.resize(traj_size);
        std::transform(tracktraj.Flags().begin(), tracktraj.Flags().end(), tracktraj_flaghitindices.begin(),
            [](auto& x) {  return x.fromHit(); } );

        std::vector<std::array<bool,tpf_maxflags>> tracktraj_flagstates;
        tracktraj_flagstates.resize(traj_size);
        /* convert tracktrajflags to a vector of flagstates, where each flagstate vector element is a bool array
           indicating whether the flag corresponding to the (flag) index is set.
           (Note that index of the element in the bool array corresponds to the flag with the flagindex.) */
        std::transform(tracktraj_flagstates.begin(), tracktraj_flagstates.end(), tracktraj_flagstates.begin(),
            [&](auto& x) {
            std::array<bool, tpf_maxflags> tpf_as_bools;
            const recob::TrajectoryPointFlags& tpf = tracktraj.FlagsAtPoint(&x - &tracktraj_flagstates[0]);
            for(tpf_flagindex_type tpf_flag_idx=0; tpf_flag_idx<tpf_maxflags; tpf_flag_idx++){
            /* convert flagindex_t to size_t to index the tpf_as_bools array! */
            tpf_as_bools[static_cast<size_t>(tpf_flag_idx)] = tpf.isDefined(tpf_flag_idx) ? tpf.test(tpf_flag_idx) : false;
            }
            return tpf_as_bools;
            });

        ar << pid << chi2 << ndof << id << fcvtx_arr << fcend_arr << tracktraj_flaghitindices << tracktraj_flagstates << traj_size << traj_pos << traj_momenta << traj_hasmomenta;
      }

    template <class Archive>
      void load(Archive &ar, recob::Track &t, const unsigned int) {
        int pid;
        float chi2;
        int ndof;
        int id;

        std::array<Double32_t, 15> fcvtx_arr;
        std::array<Double32_t, 15> fcend_arr;

        std::vector<recob::TrajectoryPointFlags::HitIndex_t> tracktraj_flaghitindices;
        std::vector<std::array<bool, tpf_maxflags>> tracktraj_flagstates;

        std::vector<std::array<root_pvscalar_type,3>> traj_pos;
        std::vector<std::array<root_dvscalar_type,3>> traj_momenta;

        size_t traj_size;
        bool traj_hasmomenta;

        ar >> pid >> chi2 >> ndof >> id >> fcvtx_arr >> fcend_arr >> tracktraj_flaghitindices >> tracktraj_flagstates >> traj_size>> traj_pos >> traj_momenta >> traj_hasmomenta;

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

        t = recob::Track(
            recob::TrackTrajectory(
              std::move(recob::tracking::convertCollToPoint(traj_pos)),
              std::move(recob::tracking::convertCollToVector(traj_momenta)),
              std::move(tracktraj_flags),
              traj_hasmomenta),
            pid, chi2, ndof,
            recob::tracking::SMatrixSym55(fcvtx_arr.begin(), fcvtx_arr.end()),
            recob::tracking::SMatrixSym55(fcend_arr.begin(), fcend_arr.end()),
            id);
      }

  } // namespace serialization

} // namespace boost
BOOST_SERIALIZATION_SPLIT_FREE(recob::Track)
#endif
