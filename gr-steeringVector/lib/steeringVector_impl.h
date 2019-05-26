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
This block applies the expected phase offset between the antennas of an Uniformly spaced linear array. 
When a recalibration signal comes in at an angle less than 90°, the phase differences between the antennas should not 
be 0, as the first antenna will receive this signal at a earlier phase than the next one. 


ArraySpacing: Amount of wavelength between two antennas.
ArrayLength: currently equal to position in antenna as block is not dynamically sized. 
vectorLength: length of the vector if not using streams.
refAngle: angle of the transmitter ~ the antenna array. 
*/

#ifndef INCLUDED_STEERINGVECTOR_STEERINGVECTOR_IMPL_H
#define INCLUDED_STEERINGVECTOR_STEERINGVECTOR_IMPL_H

#include <steeringVector/steeringVector.h>

namespace gr {
  namespace steeringVector {

    class steeringVector_impl : public steeringVector
    {
     private:
      float pi;
      float factor;
      int d_which;
      int d_num;
      float d_avg;
      float* d_results;

      gr_complex* d_buffer;

      int d_vectorLength;
      float d_arraySpacing;
      int d_arrayLength;
      int d_refAngle;

      void recalc_msg(pmt::pmt_t msg);
      float calcOffset(const gr_complex* &in);
     public:
      steeringVector_impl(int refAngle, int vectorLength, int arrayLength,float arraySpacing);
      ~steeringVector_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_STEERINGVECTOR_IMPL_H */

