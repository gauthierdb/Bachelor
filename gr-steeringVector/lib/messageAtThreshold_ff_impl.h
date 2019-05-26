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
The sample offset will be sent whenever the first rising edge of the incoming float occurs (threshold reached) and will only be sent again whenever
set_recalcSample gets a 'True' value passed (by double clicking a checkbox e.g.).
The incoming float should come from a threshold block and only have '0.0' and '1.0' outputs possible. 
The phase offset will be sent every (phaseThresholdCount) rising edges of the threshold. If d_recalcPhase gets a False value, the messages will not send. 

*/

#ifndef INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_FF_IMPL_H
#define INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_FF_IMPL_H

#include <steeringVector/messageAtThreshold_ff.h>

namespace gr {
  namespace steeringVector {

    class messageAtThreshold_ff_impl : public messageAtThreshold_ff
    {
     private:
      pmt::pmt_t d_msg;
      const pmt::pmt_t d_port_delay;
      const pmt::pmt_t d_port_phase;

			
      bool d_recalcPhase;		// should phase offset be recalibrated each (1/phaseThresholdCount) threshold? 
			bool d_recalcSample;	// should sample offset be recalibrated on next threshold
			int d_iterationCount;
			int d_initial_state;
			int d_phaseThresholdCount;

			int d_last_state;
			int counter;
			


     public:
      messageAtThreshold_ff_impl(bool recalcPhase,bool recalcSample,int iterationCount,int initial_state,int phaseThresholdCount);
      ~messageAtThreshold_ff_impl();

			void set_recalcPhase(bool b);
			void set_recalcSample(bool b);

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_MESSAGEATTHRESHOLD_FF_IMPL_H */

