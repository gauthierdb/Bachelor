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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#include <gnuradio/io_signature.h>
#include "phaseOffset_impl.h"
#include <gnuradio/math.h>
#include <volk/volk.h>

namespace gr {
  namespace steeringVector {

    phaseOffset::sptr
    phaseOffset::make(int vector_length, int vector_count,bool isActive)
    {
      return gnuradio::get_initial_sptr
        (new phaseOffset_impl(vector_length, vector_count, isActive));
    }

    /*
     * The private constructor
     */
    phaseOffset_impl::phaseOffset_impl(int vector_length,int vector_count,bool isActive)
      : gr::sync_block("phaseOffset",
              gr::io_signature::make(2, 2, sizeof(gr_complex)*vector_length),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
				d_port_in(pmt::mp("recalc")),
				d_port_out(pmt::mp("multiplyConst"))
    {
			const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
			set_alignment(std::max(1,alignment_multiple));	
			
			d_vector_length = vector_length;
			d_vector_count = vector_count;
			d_isActive = isActive;
			d_which = 0;
			d_shift = (0,0);
			d_results = new gr_complex[d_vector_count];

			d_buff = new gr_complex[d_vector_length];


			message_port_register_in(d_port_in);
			message_port_register_out(d_port_out);
			set_msg_handler(pmt::mp("recalc"), boost::bind(&phaseOffset_impl::recalc_msg, this, _1));
     }

    /*
     * Our virtual destructor.
     */
    phaseOffset_impl::~phaseOffset_impl()
    {
			delete[] d_results;
			delete[] d_buff;
    }
		


    void phaseOffset_impl::recalc_msg(pmt::pmt_t msg) {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t key = pmt::car(msg);
            pmt::pmt_t val = pmt::cdr(msg);
            if (pmt::eq(key, pmt::intern("recalc"))) {
                if (pmt::is_integer(val)) {
                    if (d_which > d_vector_count) {
                        d_vector_count = pmt::to_long(val);
                        delete[] d_results;
                        d_results = new gr_complex[d_vector_count];
                        d_which = 0;
                    }
                }
            } else {
                GR_LOG_WARN(d_logger, boost::format("Sample Offset Recalcuate message must have the key = 'recalc'; got '%1%'.") % pmt::write_string(key));
            }
        } else {
            GR_LOG_WARN(d_logger, "Sample Offset Recalculate message must be a key:value pair where the key is 'recalc'.");
        }
    }

    gr_complex phaseOffset_impl::avgPhaseDiff(const gr_complex* &in0, const gr_complex* &in1){
				gr_complex tempComp;
        volk_32fc_x2_conjugate_dot_prod_32fc(&tempComp, in0, in1, d_vector_length);
				
				float real = tempComp.real()/d_vector_length;		// needed?
				float imag = tempComp.imag()/d_vector_length;		// needed?
				return gr_complex(real, imag);									// needed? same results when just passing tempComp
				//return tempComp;		// shouldn't this be divided over d_vector_length? cause dot product is the sum of all the multiply conjugates 
		
    }



    int
    phaseOffset_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {	
      const gr_complex *in0 = (const gr_complex *) input_items[0];
      const gr_complex *in1 = (const gr_complex *) input_items[1];
      gr_complex *out = (gr_complex *) output_items[0];

      for(int i = 0; i < noutput_items; i++){
				if(d_isActive){	
					if(d_which < d_vector_count){	// Still got more vectors to do
							d_results[d_which] = avgPhaseDiff(in0,in1);
							d_which++;
					}
					if(d_which == d_vector_count){

							// Sort array based on phase -> Computationally heavy? Find other method. 
							std::sort(d_results, d_results+d_vector_count, [](gr_complex a, gr_complex b){return fast_atan2f(a) < fast_atan2f(b);});

							// Print average phase diff for each vector in d_vector_count 
							// (Computationally heavy -> calls fast_atan2f for all elements)
/*
							printf("\nAverage phase difference: [");
							
							for(int j = 0; j < d_vector_count; j++){
								printf("%lf, ", fast_atan2f(d_results[j]));		
							}

							printf("]\n");
*/							

							d_shift = d_results[d_vector_count/2];


							// only apply when threshold is exceeded
							if(real(d_shift) != 0.0 && imag(d_shift) != 0.0){
								message_port_pub(d_port_out, pmt::cons(pmt::mp("multiplyConst"),pmt::mp(conj(d_shift))));
								//printf("Multiply by [%f : %f]\n",real(d_shift),imag(d_shift));
							}else{
								//printf("DID NOT Multiply by [%f : %f]\n",real(d_shift),imag(d_shift));								
							}
							
							d_which++;		// filled d_vector_count
					}
					
					out[i] = d_shift;
					in0 += d_vector_length;
					in1 += d_vector_length;
			 }else{		// Not active : output last result
					
					//out[i] = d_results[d_vector_count/2];
			 }
			


			}
			//std::fill(out,out+noutput_items,d_results[d_vector_count/2]);

      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

