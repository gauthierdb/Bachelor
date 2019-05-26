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

#include <gnuradio/math.h>

#include <gnuradio/io_signature.h>
#include "steeringVector_impl.h"

namespace gr {
  namespace steeringVector {

    steeringVector::sptr
    steeringVector::make(int refAngle, int vectorLength,int arrayLength,float arraySpacing)
    {
      return gnuradio::get_initial_sptr
        (new steeringVector_impl(refAngle, vectorLength, arrayLength, arraySpacing));
    }

    /*
     * The private constructor
     */
    steeringVector_impl::steeringVector_impl(int refAngle, int vectorLength, int arrayLength, float arraySpacing)
      : gr::sync_block("steeringVector",
              gr::io_signature::make(1, 1, sizeof(gr_complex)*vectorLength),
              gr::io_signature::make(1, 1, sizeof(gr_complex)*vectorLength)),
	d_vectorLength(vectorLength),
	d_arraySpacing(arraySpacing),
	d_arrayLength(arrayLength),
	d_refAngle(refAngle)

    {
        pi = 3.14159265359;
        factor = (float) 2*pi*d_arraySpacing*cos(d_refAngle*pi/180)*d_arrayLength;
/*
        d_which = 0;	// Index at which vector we're currently at
	d_num = 10;	// Number of vectors to take the mean of. 
	d_avg = 0;	// Final result, this is the average of d_results
	
	d_results = new float[d_num];
	d_buffer = new gr_complex[d_vectorLength];
*/

	message_port_register_in(pmt::mp("recalc"));
        set_msg_handler(pmt::mp("recalc"), boost::bind(&steeringVector_impl::recalc_msg, this, _1));

    }

    /*
     * Our virtual destructor.
     */
    steeringVector_impl::~steeringVector_impl()
    {
    }

    void steeringVector_impl::recalc_msg(pmt::pmt_t msg) {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t key = pmt::car(msg);
            pmt::pmt_t val = pmt::cdr(msg);
            if (pmt::eq(key, pmt::intern("recalc"))) {
                if (pmt::is_integer(val)) {
                    if (d_which > d_num) {
                        d_num = pmt::to_long(val);
                        delete[] d_results;
                        d_results = new float[d_num];
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

// Find average phase offset for one vector and store in result[vectorCount]. 	
    float steeringVector_impl::calcOffset(const gr_complex* &in){ 	
    
      	const gr_complex imag(0,-1);		
      	gr_complex exp1 = std::exp(imag*factor);	// e^[j*2Pi*dArray*cos(angle)*n]

	//memcpy(d_buffer, in, sizeof(gr_complex)*d_vectorLength);
	float cumulative = 0;	
	for(int i = 0; i < d_vectorLength; i++){	
	    cumulative += gr::fast_atan2f(in[i]*exp1);	
	}
	return(cumulative/d_vectorLength);
    }

    int
    steeringVector_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex*) output_items[0];

      // Do <+signal processing+>

      int noi = noutput_items * d_vectorLength;
      for(int i = 0; i < noi; i++){
	//printf("%d",d_which);	
/*
	if(d_which < d_num){	// If we still have vectors to go
	    d_results[d_which] = (float) calcOffset(in);
	    d_which++;
	}
	if(d_which == d_num){	// Reached last vector -> find the median of these values
              // do median
              //std::sort(d_results, d_results+d_num);
              printf("\nresults: [");
              for (int j=0; j<d_num; j++) {
                  printf("%d, ", d_results[j]);
              }
              printf("]\n");
              d_avg = d_results[d_num/2];
              d_which++;
          }
*/
      	const gr_complex imag(0,-1);
      	gr_complex exp1 = std::exp(imag*factor);	// complex conjugate of (e^[j*2Pi*dArray*cos(angle)*n])
	out[i] = in[i]*exp1;
	

      }
// Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

