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
Explanation
PhaseOffset calculates the necessary phase shift a channel has to undergo to be in phase synchronization with the reference channel. 
The offset is calculated using the dot product (which is the sum of the multiplication with the complex conjugate) which is then divided over the channel's vector length, gives the average phase offset. 
This operation is done 'vector_count' times and the results are sorted by phase and the mean is taken. 

Calculation is done each time a 'recalc' message comes in. This message has key 'recalc' and value equal to the iterations necessary. 
The messages are provided by the 'messageAtThreshold' block.  

This block provides good results even when decimation is done. Recommended iteration count for a Dash-7 signal lies around 10 to 20.

To do:
* Currently, the results are sorted by phase, which means that for each operation, each result is converted to phase. 
  This might be to computationally intensive. A study needs to be conducted on this. 

* Make this block dynamically sized, including decimation and multiply complex in the code, eliminating the need for hier-block. 
  Currently, output is unused and message passing to multiply complex is used. Should change to directly output correct stream. 
  Problem: If vector is directly outputted from here, decimation causes vector size to be smaller. Reconstruction means loss of data. 
           Decimation should only happen for the calculation of the offset, not for the application to the stream
  

* While this can improve calculation time, 'SampleOffsetMulti' block is still bottlenecking w.r.t computational power
  So priority is optimising that first. 

*/

#ifndef INCLUDED_STEERINGVECTOR_PHASEOFFSET_IMPL_H
#define INCLUDED_STEERINGVECTOR_PHASEOFFSET_IMPL_H

#include <steeringVector/phaseOffset.h>

namespace gr {
  namespace steeringVector {

    class phaseOffset_impl : public phaseOffset
    {
     private:
	int d_vector_length;
	int d_vector_count;
	int d_which;
	bool d_isActive;    
	gr_complex d_shift;  

	gr_complex* d_results;
	gr_complex* d_buff;

      	const pmt::pmt_t d_port_in;
	const pmt::pmt_t d_port_out;
	
	void recalc_msg(pmt::pmt_t msg);
	gr_complex avgPhaseDiff(const gr_complex* &in0, const gr_complex* &in1);
     public:
      phaseOffset_impl(int vector_length,int vector_count, bool isActive);
      ~phaseOffset_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_PHASEOFFSET_IMPL_H */

