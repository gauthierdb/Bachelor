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
#include "messageAtThreshold_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace steeringVector {

    messageAtThreshold::sptr
    messageAtThreshold::make(bool isActive,int vector_count,float threshLow,float threshHigh,float initial_state)
    {
      return gnuradio::get_initial_sptr
        (new messageAtThreshold_impl(isActive, vector_count, threshLow, threshHigh, initial_state));
    }

    /*
     * The private constructor
     */
    messageAtThreshold_impl::messageAtThreshold_impl(bool isActive,int vector_count,float threshLow,float threshHigh, float initial_state)
      : gr::sync_block("messageAtThreshold",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(0,0,0)),
				d_port_delay(pmt::mp("recalcDelay")),
				d_port_phase(pmt::mp("recalcPhase")),
				d_msg(pmt::cons(pmt::mp("recalc"),pmt::mp(vector_count))),
				d_isActive(isActive),
				d_vector_count(vector_count),
				d_threshLow(threshLow),
				d_threshHigh(threshHigh),
				d_last_state(initial_state)
    {
			d_calibPhase = 5;		// A counter for when the delay needs to be calculated
			message_port_register_out(d_port_delay);
    	message_port_register_out(d_port_phase);

			const int alignment_multiple = volk_get_alignment();
			set_alignment(std::max(1,alignment_multiple));
    }

    /*
     * Our virtual destructor.
     */
    messageAtThreshold_impl::~messageAtThreshold_impl()
    {
			delete[] d_inFloat;
    }

    void messageAtThreshold_impl::set_isActive(bool b){
			//d_isActive = b;
			if(b == true){
				d_calibPhase = 5;
			}
    }


    int
    messageAtThreshold_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
			
      const gr_complex *in = (const gr_complex *) input_items[0];

      if(d_isActive){
      	d_inFloat = new float[noutput_items];
				volk_32fc_magnitude_32f_u(d_inFloat, in, noutput_items);
	
				for(int i = 0; i < noutput_items; i++){			// Might have to rewrite so that phase calib will be calculated more often! 

					if(d_inFloat[i] > d_threshHigh){	

						if(d_last_state == 0.0){		// isActive is true + signal went from threshLow to threshHigh  
							if(d_calibPhase == 5){
								message_port_pub(d_port_delay, d_msg);
								d_calibPhase--;	// giving a slight delay to give the delay some time to calculate
							}else if(d_calibPhase == 0){
								message_port_pub(d_port_phase, d_msg);
								d_calibPhase = 4;			// reset counter to not calibrate each peak. if 5 -> will calibrate samples. if lower, more phase calib
								//d_isActive = false;		// uncomment if you do not want recalibration of phase each threshold
							}else{
								d_calibPhase--;
							}
						}
						
						d_last_state = 1.0;
					}
				  else if(d_inFloat[i] < d_threshLow){

	    			d_last_state = 0.0;
	  			}
	  
				}
				delete[] d_inFloat;
      }

      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

