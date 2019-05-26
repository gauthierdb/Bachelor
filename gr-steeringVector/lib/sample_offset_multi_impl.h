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
This block is heavily inspired on Sam Whiting's gnuradio-doa project found here https://github.com/samwhiting/gnuradio-doa

It will calculate sample offset using cross correlation of zero-padded vectors. This process is done for each channel relative to the first. 
Calculations are repeated d_vector_count times and the mean cross-correlation peak (which is the delay of the two channels) is calculated. 
Each delay is compensated by the last one to only apply positive delays and is sent as a message to the 'delay' block. 
When an incoming message consisting of key : 'recalc' and value equal to the amount of calculations done (d_vector_count),
the offset is recalculated. 

*/

#ifndef INCLUDED_STEERINGVECTOR_SAMPLE_OFFSET_MULTI_IMPL_H
#define INCLUDED_STEERINGVECTOR_SAMPLE_OFFSET_MULTI_IMPL_H

#include <steeringVector/sample_offset_multi.h>
#include <gnuradio/fft/fft.h>

namespace gr {
  namespace steeringVector {

    class sample_offset_multi_impl : public sample_offset_multi
    {
     private:
      int d_vector_length;
			int d_vector_count;
			int d_num_inputs;
			int d_fft_width;
			int d_which;

			std::vector<int*> d_results_vector;
			std::vector<const gr_complex*> input_vector;

			int* d_means;

			pmt::pmt_t d_msg;
			std::vector<pmt::pmt_t> d_ports;
      pmt::pmt_t d_port_in;
			pmt::pmt_t d_port_out;
	

      gr::fft::fft_complex* d_fft;
      gr::fft::fft_complex* d_ifft;
			gr_complex* d_buffer_ref_fft;
      gr_complex* d_buffer_in_fft;
      gr_complex* d_buffer;
      gr_complex* d_buffer2;

			int get_shift(const gr_complex* &in);
			void recalc_msg(pmt::pmt_t msg);

     public:
      sample_offset_multi_impl(int vector_length, int vector_count, int num_inputs);
      ~sample_offset_multi_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_SAMPLE_OFFSET_MULTI_IMPL_H */

