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
#include "multiplyComplex_impl.h"

namespace gr {
  namespace steeringVector {

    multiplyComplex::sptr
    multiplyComplex::make()
    {
      return gnuradio::get_initial_sptr
        (new multiplyComplex_impl());
    }

    /*
     * The private constructor
     */
    multiplyComplex_impl::multiplyComplex_impl()
      : gr::sync_block("multiplyComplex",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex))),
				d_port_in(pmt::mp("multiplyConst"))
    {
			message_port_register_in(d_port_in);	      
			set_msg_handler(d_port_in, boost::bind(&multiplyComplex_impl::multiply_msg, this, _1));
			complexConst = (1,1);

		}

    /*
     * Our virtual destructor.
     */
    multiplyComplex_impl::~multiplyComplex_impl()
    {
    }

	
    void multiplyComplex_impl::multiply_msg(pmt::pmt_t msg) {
        if (pmt::is_pair(msg)) {
            pmt::pmt_t key = pmt::car(msg);
            pmt::pmt_t val = pmt::cdr(msg);
            if (pmt::eq(key, pmt::mp("multiplyConst"))) {
                if (pmt::is_complex(val)) {
                  complexConst = (gr_complex) pmt::to_complex(val);
	              }else{
									printf("Not a complex input!");
								}
            } else {
                GR_LOG_WARN(d_logger, boost::format("Sample Offset Recalcuate message must have the key = 'multiplyConst'; got '%1%'.") % pmt::write_string(key));
            }
        } else {
            GR_LOG_WARN(d_logger, "Sample Offset Recalculate message must be a key:value pair where the key is 'multiplyConst'.");
        }
    }



    int
    multiplyComplex_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];

      // Do <+signal processing+>
			for(int i = 0; i < noutput_items; i++){
				out[i] = in[i]*complexConst;
			}
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace steeringVector */
} /* namespace gr */

