//
// This code generates hits and tracks objects that 
// we can use in testing 
//
//

#include "lardataobj/RecoBase/Hit.h"

#include <iostream>

using namespace std;

recob::Hit make_hit(raw::ChannelID_t chl) {
  raw::ChannelID_t channel = chl;
  raw::TDCtick_t start_tick = 2;
  raw::TDCtick_t end_tick = 3;
  float peak_time = 1.5;
  float sigma_peak_time = 2.5;
  float rms = 3.5;
  float peak_amplitude = 4.5;
  float sigma_peak_amplitude = 5.5;
  float summedADC = 6.5;
  float hit_integral = 7.5;
  float hit_sigma_integral = 8.5;
  short int multiplicity = 4;
  short int local_index = 5;
  float goodness_of_fit = 9.5;
  int dof = -1;
  geo::View_t view = geo::k3D;
  geo::SigType_t signal_type = geo::kCollection;
  geo::WireID wireID(geo::PlaneID(geo::TPCID(geo::CryostatID(11), 14), 13), 12);
  recob::Hit h(channel,
               start_tick,
               end_tick,
               peak_time,
               sigma_peak_time,
               rms,
               peak_amplitude,
               sigma_peak_amplitude,
               summedADC,
               hit_integral,
               hit_sigma_integral,
               multiplicity,
               local_index,
               goodness_of_fit,
               dof,
               view,
               signal_type,
               wireID);
  return h;
}
