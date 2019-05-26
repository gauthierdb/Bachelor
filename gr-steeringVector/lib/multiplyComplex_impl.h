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
The multiplyComplex block will multiply a complex stream with a complex value passed by message. 
The message should have a key equal to 'multiplyConst' and a value containing the complex multiplication value. 




*/
#ifndef INCLUDED_STEERINGVECTOR_MULTIPLYCOMPLEX_IMPL_H
#define INCLUDED_STEERINGVECTOR_MULTIPLYCOMPLEX_IMPL_H

#include <steeringVector/multiplyComplex.h>

namespace gr {
  namespace steeringVector {

    class multiplyComplex_impl : public multiplyComplex
    {
     private:
			const pmt::pmt_t d_port_in;
			void multiply_msg(pmt::pmt_t msg);
			gr_complex complexConst;
     public:
      multiplyComplex_impl();
      ~multiplyComplex_impl();

      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace steeringVector
} // namespace gr

#endif /* INCLUDED_STEERINGVECTOR_MULTIPLYCOMPLEX_IMPL_H */

