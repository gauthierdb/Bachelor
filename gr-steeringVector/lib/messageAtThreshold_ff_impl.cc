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
#include "messageAtThreshold_ff_impl.h"

namespace gr {
  namespace steeringVector {

    messageAtThreshold_ff::sptr
    messageAtThreshold_ff::make(bool recalcPhase,bool recalcSample,int iterationCount,int initial_state,int phaseThresholdCount)
    {
      return gnuradio::get_initial_sptr
        (new messageAtThreshold_ff_impl(recalcPhase, recalcSample, iterationCount, initial_state, phaseThresholdCount));
    }

    /*
     * The private constructor
     */
    messageAtThreshold_ff_impl::messageAtThreshold_ff_impl(bool recalcPhase,bool recalcSample,int iterationCount,int initial_state,int phaseThresholdCount)
      : gr::sync_block("messageAtThreshold_ff",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(0,0,0)),
			d_port_delay(pmt::mp("recalcDelay")),
			d_port_phase(pmt::mp("recalcPhase")),
			d_msg(pmt::cons(pmt::mp("recalc"),pmt::mp(iterationCount))),
			d_recalcPhase(recalcPhase),
			d_recalcSample(recalcSample),
			d_last_state(initial_state),
			d_iterationCount(iterationCount),
			d_phaseThresholdCount(phaseThresholdCount)
    {
			message_port_register_out(d_port_delay);
    	message_port_register_out(d_port_phase);
			counter = d_phaseThresholdCount;
		}

    /*
     * Our virtual destructor.
     */
    messageAtThreshold_ff_impl::~messageAtThreshold_ff_impl()
    {
    }


		void messageAtThreshold_ff_impl::set_recalcPhase(bool b){
			d_recalcPhase = b;
		}

		void messageAtThreshold_ff_impl::set_recalcSample(bool b){
			d_recalcSample = b;
		}



    int
    messageAtThreshold_ff_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const float *in = (const float *) input_items[0];


			for(int i = 0; i < noutput_items; i++){		// find other way? 
				if(in[i] == 0.0){
					d_last_state = 0;
				}else if(in[i] == 1.0){
					if(d_last_state == 0){		// low to high toggle
						
						//printf("Threshold Reached after toggle!\n");
						if(d_recalcSample){
							message_port_pub(d_port_delay, d_msg);
							d_recalcSample = false;		// should only happen once if not manual reset 
						}

						if(counter > 0){
							counter--;
						}else{ 								// phaseThresholdCount thresholds exceeded, send phase recalib & reset counter
							if(d_recalcPhase){
								//printf("Phase recalib message sent!\n");
								message_port_pub(d_port_phase, d_msg);
								counter = d_phaseThresholdCount;
							}
						}						
					}
					d_last_state = 1;
				}else{
					printf("in is %f\n",in[i]);	// should not happen. Threshold block should only give 0 or 1 
				}
			}

      // Do <+signal processing+>

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

