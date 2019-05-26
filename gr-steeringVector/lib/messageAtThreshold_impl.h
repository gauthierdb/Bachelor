/* -*- c++ -*- */
/* 
 * Copyright 2019 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

/*
Explanation:
This block will send out two messages, one for phase offset recalculation and one for sample offset recalculation, each on a separate message source. 
The incoming stream should be complex data which is converted to magnitude using volk libraries.
The sample offset will be sent whenever the first rising edge of the magnitude occurs (threshold reached) and will only be sent again whenever
isActive is True. 
The phase offset will be sent every 5 rising edges of the threshold. 
*/

#ifndef INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_IMPL_H
#define INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_IMPL_H

#include <steeringVector/messageAtThreshold.h>

namespace gr {
  namespace steeringVector {

    class messageAtThreshold_impl : public messageAtThreshold
    {
     private:
      pmt::pmt_t d_msg;
      const pmt::pmt_t d_port_delay;
      const pmt::pmt_t d_port_phase;

      bool d_isActive;
			int d_calibPhase;		// if false, calib delay. If true, calib phase
      int d_vector_count;
      float d_threshLow;
      float d_threshHigh;
      float d_last_state;
      float* d_inFloat;
			

     public:
      messageAtThreshold_impl(bool isActive, int vector_count, float threshLow, float threshHigh, float initial_state);
      ~messageAtThreshold_impl();
			void set_isActive(bool d);
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_IMPL_H */

