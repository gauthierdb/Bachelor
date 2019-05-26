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
#include "sample_offset_multi_impl.h"
#include <algorithm>
#include <volk/volk.h>


namespace gr {
  namespace steeringVector {

    sample_offset_multi::sptr
    sample_offset_multi::make(int vector_length,int vector_count,int num_inputs)
    {
      return gnuradio::get_initial_sptr
        (new sample_offset_multi_impl(vector_length, vector_count, num_inputs));
    }

    /*
     * The private constructor
     */
    sample_offset_multi_impl::sample_offset_multi_impl(int vector_length,int vector_count,int num_inputs)
      : gr::sync_block("sample_offset_multi",
              gr::io_signature::make(2, 8, sizeof(gr_complex)*vector_length),
              gr::io_signature::make(0,0,0)),
				d_port_in(pmt::mp("recalc"))

     {
			// Fill d_ports with each output port and map it to a output 
			char buffer[20];
			for(int i = 0; i < num_inputs; i++){
				sprintf(buffer, "recalcOut%d",i);
				d_port_out = pmt::mp(buffer);
				message_port_register_out(d_port_out);
				d_ports.push_back(d_port_out);  
			}			
		
			d_vector_length = vector_length;
			d_vector_count = vector_count;
			d_num_inputs = num_inputs;
			d_fft_width = d_vector_length*2-1;
			d_which = 0;

			d_buffer_in_fft = new gr_complex[d_fft_width];
			d_buffer_ref_fft = new gr_complex[d_fft_width];
			d_buffer = new gr_complex[d_fft_width];

      d_fft = new gr::fft::fft_complex(d_fft_width, true, 1);
      d_ifft = new gr::fft::fft_complex(d_fft_width, false, 1);

			// fill vector of pointers with new int array pointers
     	for(int i = 0; i < d_num_inputs; i++){
				int* d_results = new int[d_vector_count];		// remember to delete all these
				d_results_vector.push_back(d_results);
			}
				
			d_means = new int[d_num_inputs];
			
			d_msg = pmt::cons(pmt::mp("recalc"),pmt::mp(vector_count));
			
		  message_port_register_in(d_port_in);
      set_msg_handler(d_port_in, boost::bind(&sample_offset_multi_impl::recalc_msg, this, _1));


		}

    /*
     * Our virtual destructor.
     */
    sample_offset_multi_impl::~sample_offset_multi_impl()
    {
			delete[] d_fft;
			delete[] d_ifft;
			
			for(int i = 0; i < d_num_inputs; i++){
				delete[] d_results_vector[i];
				delete[] input_vector[i];			//??
			}
			

			delete[] d_means;

			delete[] d_buffer_ref_fft;
			delete[] d_buffer_in_fft;
			delete[] d_buffer;

    }

		void sample_offset_multi_impl::recalc_msg(pmt::pmt_t msg) {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t key = pmt::car(msg);
            pmt::pmt_t val = pmt::cdr(msg);
            if (pmt::eq(key, pmt::intern("recalc"))) {
                if (pmt::is_integer(val)) {
                    if (d_which > d_vector_count) {		// if done with previous calibration
											 for(int i = 0; i < d_num_inputs; i++){
												 delete[] d_results_vector[i];
											 }
												d_results_vector.clear();

												d_vector_count = pmt::to_long(val);

												for(int i = 0; i < d_num_inputs; i++){
													int* d_results = new int[d_vector_count];		// remember to delete all these
													d_results_vector.push_back(d_results);
												}
                       
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
		
			int sample_offset_multi_impl::get_shift(const gr_complex* &in) {
        // zero pad in1 and take fft
        memcpy(d_fft->get_inbuf(), in, sizeof(gr_complex)*d_vector_length);
        bzero(d_fft->get_inbuf()+d_vector_length, sizeof(gr_complex)*(d_vector_length-1));
        d_fft->execute();
        memcpy(d_buffer_in_fft, d_fft->get_outbuf(), sizeof(gr_complex)*d_fft_width);

				volk_32fc_x2_multiply_conjugate_32fc(d_buffer, d_buffer_ref_fft, d_buffer_in_fft, d_fft_width);

        // take ifft
        memcpy(d_ifft->get_inbuf(), d_buffer, sizeof(gr_complex)*d_fft_width);
        d_ifft->execute();
        memcpy(d_buffer, d_ifft->get_outbuf(), sizeof(gr_complex)*d_fft_width);

        // find argmax
        int argmax = 0;
        for (int i=1; i<d_fft_width; ++i) {
            if (norm(d_buffer[argmax]) < norm(d_buffer[i])) {
                argmax = i;
            }
        }

        return (argmax < d_fft_width/2) ? argmax : argmax - d_fft_width;
    }





    int
    sample_offset_multi_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
			for(int i = 1; i < d_num_inputs; i++){
				const gr_complex * in = (const gr_complex *) input_items[i];
				input_vector.push_back(in);																		// careful! [input_vector] indexes from 0 to 6 for input_items 1 to 7 !!! 
			}
			const gr_complex * inRef = (const gr_complex *) input_items[0];	
		
			for(int it = 0; it < noutput_items; it++){ 
				
				if(d_which < d_vector_count){		// Still iterations to go

					//Make the reference fft buffer 
					memcpy(d_fft->get_inbuf(), inRef, sizeof(gr_complex)*d_vector_length);
				  bzero(d_fft->get_inbuf()+d_vector_length, sizeof(gr_complex)*(d_vector_length-1));
				  d_fft->execute();
				  memcpy(d_buffer_ref_fft, d_fft->get_outbuf(), sizeof(gr_complex)*d_fft_width);
					
					// calculate xCorr for each other input		

					for(int i = 1; i < d_num_inputs; i++){
						d_results_vector[i][d_which] = get_shift(input_vector[i-1]);	// for i-1 see note at init of input_vector
					}
					d_which++;
				}

				if(d_which == d_vector_count){		// done doing final iteration. Calculate sample offset & send delay messages
					int min = 0;
					for(int i = 1; i < d_num_inputs; i++){

						// take one int array pointer from the vector
						int* d_results = d_results_vector[i];		
	
						// sort it
						std::sort(d_results, d_results+d_vector_count);		

						// print each value of it
						printf("Results %d : [",i);
						for(int j = 0; j < d_vector_count; j++){		
							printf("%d, ", d_results[j]);					
						}
						printf("]\n");

						// store its mean in d_means
						d_means[i] = d_results[d_vector_count/2];		
				
						// Check if this is the minimum mean					
						if(d_means[i] < min){
							min = d_means[i];
						}
						
					}
					d_means[0] = 0;


					printf("@@@@@@@@@@\n");
					for(int i = 0; i < d_num_inputs; i++){
						d_means[i] -= min;
						printf("Delay for channel %d is %d\n",i, d_means[i]);
					}

					for(int i = 0; i < d_num_inputs; i++){
						message_port_pub(d_ports[i], pmt::mp(d_means[i]));
					}					
					d_which++;
				}



				inRef += d_vector_length;
				for(int i = 0; i < d_num_inputs-1; i++){
					input_vector[i] += d_vector_length;
				}
				


			}

			//for(int i = 0; i < d_num_inputs-1; i++){
				//delete[] input_vector[i];
			//}
			input_vector.clear();

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

